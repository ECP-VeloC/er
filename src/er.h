#ifndef ER_H
#define ER_H

#include "mpi.h"

/** \defgroup er ER
 *  \brief Encode and Rebuild
 *
 * ER is the abstraction of shuffile and redset into a single
 * interface, SCR and VeloC use both and er to simplify the rebuilding
 * steps. On a restart, shuffile is used to first move files back to
 * owning ranks, depending on new rank-to-node mapping. Then redset is
 * used to rebuild missing files after the shuffle. */

/** \file er.h
 *  \ingroup er
 *  \brief encoding and redundancy library */

/* enable C++ codes to include this header directly */
#ifdef __cplusplus
extern "C" {
#endif

#define ER_SUCCESS (0)
#define ER_FAILURE (1)

#define ER_DIRECTION_ENCODE  (1)
#define ER_DIRECTION_REBUILD (2)
#define ER_DIRECTION_REMOVE  (3)

int ER_Init(
  const char* conf_file /**< [IN] - path to configuration file (can be NULL for default) */
);

int ER_Finalize(void);

/** defines a redundancy scheme, returns scheme id as integer */
int ER_Create_Scheme(
  MPI_Comm comm,              /**< [IN] - communicator of processes participating in scheme */
  const char* failure_domain, /**< [IN] - processes with same value of failure_domain are assumed to fail at the same time */
  int data_blocks,            /**< [IN] - number of original data blocks */
  int erasure_blocks          /**< [IN] - number of erasure blocks to be generated */
);

int ER_Free_Scheme(
  int scheme_id /**< [IN] - release resources associated with specified scheme id */
);

/** create a named set, and specify whether it should be encoded, recovered, or unencoded */
int ER_Create(
  MPI_Comm comm_world, /**< [IN] - communicator of processes participating in operation */
  MPI_Comm comm_store, /**< [IN] - communicator of processes that share access to storage holding files */
  const char* name,    /**< [IN] - name of operation */
  int direction,       /**< [IN] - operation to execute: one of ER_DIRECTION constants */
  int scheme_id        /**< [IN] - redundancy scheme to be applied to this set */
);

/** adds file to specified set id */
int ER_Add(
  int set_id,      /**< [IN] - set id to add file to */
  const char* file /**< [IN] - path to file */
);

/** initiate encode/rebuild operation on specified set id */
int ER_Dispatch(
  int set_id           /**< [IN] - set id to dispatch */
);

/** tests whether ongoing dispatch operation to finish,
 * returns 1 if done, 0 otherwise */
int ER_Test(
  int set_id
);

/** wait for ongoing dispatch operation to finish */
int ER_Wait(
  int set_id
);

/** free internal resources associated with set id */
int ER_Free(
  int set_id
);

/* enable C++ codes to include this header directly */
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ER_H */
