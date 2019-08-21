#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <limits.h>
#include <unistd.h>

#include "mpi.h"
#include "rankstr_mpi.h"

#include "er.h"

#define ER_HOSTNAME (255)

int test_encode(int scheme_id, MPI_Comm world, MPI_Comm store, const char* name, int numfiles, const char** filelist)
{
  // encode files using redundancy scheme
  int rc = ER_SUCCESS;
  int set_id = ER_Create(world, store, name, ER_DIRECTION_ENCODE, scheme_id);

  int i;
  for (i = 0; i < numfiles; i++) {
    const char* file = filelist[i];
    if(ER_Add(set_id, file) == ER_FAILURE) rc = ER_FAILURE;
  }

  if(ER_Dispatch(set_id)  == ER_FAILURE) rc = ER_FAILURE;
  if(ER_Wait(set_id)  == ER_FAILURE) rc = ER_FAILURE;
  if(ER_Free(set_id)  == ER_FAILURE) rc = ER_FAILURE;
  return rc;
}

int test_rebuild_no_failure(MPI_Comm world, MPI_Comm store, const char* name)
{
  // rebuild encoded files (and redundancy data)
  int rc = ER_SUCCESS;
  int set_id = ER_Create(world, store, name, ER_DIRECTION_REBUILD, 0);
  if(ER_Dispatch(set_id)  == ER_FAILURE) rc = ER_FAILURE;
  if(ER_Wait(set_id)  == ER_FAILURE) rc = ER_FAILURE;
  if(ER_Free(set_id)  == ER_FAILURE) rc = ER_FAILURE;
  return rc;
}

int test_rebuild_one_failure_reverse(MPI_Comm world, MPI_Comm store, const char* name, int numfiles, const char** filelist)
{
  int rc = ER_SUCCESS;
  int rank, ranks;
  MPI_Comm_rank(world, &rank);
  MPI_Comm_size(world, &ranks);

  // delete files for one rank
  if (rank == 0) {
    int i;
    for (i = 0; i < numfiles; i++) {
      const char* file = filelist[i];
      unlink(file);
    }
  }

  // reverse ranks in world communicator
  MPI_Comm newworld;
  MPI_Comm_split(world, 0, ranks - rank, &newworld);

  /* create communicator of all procs on the same host */
  MPI_Comm newstore;
  char hostname[ER_HOSTNAME + 1];
  gethostname(hostname, sizeof(hostname));
  rankstr_mpi_comm_split(newworld, hostname, 0, 0, 1, &newstore);

  // rebuild encoded files (and redundancy data)
  int set_id = ER_Create(newworld, newstore, name, ER_DIRECTION_REBUILD, 0);
  if(ER_Dispatch(set_id)  != ER_FAILURE) rc = ER_FAILURE;
  if(ER_Wait(set_id)  != ER_FAILURE) rc = ER_FAILURE;
  if(ER_Free(set_id)  == ER_FAILURE) rc = ER_FAILURE;

  MPI_Comm_free(&newstore);
  MPI_Comm_free(&newworld);
  return rc;
}

int test_remove_no_failure(MPI_Comm world, MPI_Comm store, const char* name)
{
  int rc = ER_SUCCESS;
  int set_id = ER_Create(world, store, name, ER_DIRECTION_REMOVE, 0);
  if(ER_Dispatch(set_id)  == ER_FAILURE) rc = ER_FAILURE;
  if(ER_Wait(set_id)  == ER_FAILURE) rc = ER_FAILURE;
  if(ER_Free(set_id)  == ER_FAILURE) rc = ER_FAILURE;
  return rc;
}

int main (int argc, char* argv[])
{
  int rc = ER_SUCCESS;
  int res;
  MPI_Init(&argc, &argv);

  int rank, ranks;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &ranks);

  char buf[256];
  sprintf(buf, "data from rank %d\n", rank);

  char filename[256];
  sprintf(filename, "/dev/shm/testfile_%d.out", rank);
  int fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
  if (fd != -1) {
    write(fd, buf, strlen(buf));
    close(fd);
  } else {
    printf("Error opening file %s: %d %s\n", filename, errno, strerror(errno));
  }

  char hostname[ER_HOSTNAME + 1];
  gethostname(hostname, sizeof(hostname));

  /* create communicator of all procs on the same host */
  MPI_Comm comm_host;
  rankstr_mpi_comm_split(MPI_COMM_WORLD, hostname, 0, 0, 1, &comm_host);

  char rankstr[256];
  sprintf(rankstr, "%d", rank);

  const char* filelist[1] = { filename };

  if(ER_Init(NULL) != ER_FAILURE) rc = ER_FAILURE;

  char dsetname[256];
  sprintf(dsetname, "/dev/shm/timestep.%d", 1);

  int scheme_id = ER_Create_Scheme(MPI_COMM_WORLD, hostname, ranks, ranks);

  // encode files using redundancy scheme
  res = test_encode(scheme_id, MPI_COMM_WORLD, comm_host, dsetname, 1, filelist);
  if(res == ER_FAILURE) rc = ER_FAILURE;

  // rebuild encoded files (and redundancy data)
  res = test_rebuild_no_failure(MPI_COMM_WORLD, comm_host, dsetname);
  if(res == ER_FAILURE) rc = ER_FAILURE;

  // delete redundancy data added during ENCODE
  res = test_remove_no_failure(MPI_COMM_WORLD, comm_host, dsetname);
  if(res == ER_FAILURE) rc = ER_FAILURE;

  // encode files using redundancy scheme
  res = test_encode(scheme_id, MPI_COMM_WORLD, comm_host, dsetname, 1, filelist);
  if(res == ER_FAILURE) rc = ER_FAILURE;

  // rebuild encoded files (and redundancy data)
  res = test_rebuild_one_failure_reverse(MPI_COMM_WORLD, comm_host, dsetname, 1, filelist);
  if(res == ER_FAILURE) rc = ER_FAILURE;

  if(ER_Free_Scheme(scheme_id) == ER_FAILURE) rc = ER_FAILURE;

  if(ER_Finalize() == ER_FAILURE) rc = ER_FAILURE;

  unlink(filename);

  MPI_Comm_free(&comm_host);

  MPI_Finalize();

  return rc;
}
