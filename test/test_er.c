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

int main (int argc, char* argv[])
{
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

  char hostname[HOST_NAME_MAX + 1];
  gethostname(hostname, sizeof(hostname));

  /* create communicator of all procs on the same host */
  MPI_Comm comm_host;
  rankstr_mpi_comm_split(MPI_COMM_WORLD, hostname, 0, 0, 1, &comm_host);

  char rankstr[256];
  sprintf(rankstr, "%d", rank);

  const char* filelist[1] = { filename };

  ER_Init(NULL);

  char dsetname[256];
  sprintf(dsetname, "/dev/shm/timestep.%d", 1);

  int scheme_id = ER_Create_Scheme(MPI_COMM_WORLD, hostname, ranks, ranks);

  // encode files using redundancy scheme
  int set_id = ER_Create(MPI_COMM_WORLD, comm_host, dsetname, ER_DIRECTION_ENCODE, scheme_id);
  ER_Add(set_id, filename);
  ER_Dispatch(set_id);
  ER_Wait(set_id);
  ER_Free(set_id);

  // rebuild encoded files (and redundancy data)
  set_id = ER_Create(MPI_COMM_WORLD, comm_host, dsetname, ER_DIRECTION_REBUILD, 0);
  ER_Dispatch(set_id);
  ER_Wait(set_id);
  ER_Free(set_id);

  // delete redundancy data added during ENCODE
  set_id = ER_Create(MPI_COMM_WORLD, comm_host, dsetname, ER_DIRECTION_REMOVE, 0);
  ER_Dispatch(set_id);
  ER_Wait(set_id);
  ER_Free(set_id);

  ER_Free_Scheme(scheme_id);

  ER_Finalize();

  MPI_Comm_free(&comm_host);

  MPI_Finalize();

  return 0;
}
