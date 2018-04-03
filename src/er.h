/*
 * Copyright (c) 2009, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 * Written by Adam Moody <moody20@llnl.gov>.
 * LLNL-CODE-411039.
 * All rights reserved.
 * This file is part of The Scalable Checkpoint / Restart (SCR) library.
 * For details, see https://sourceforge.net/projects/scalablecr/
 * Please also read this file: LICENSE.TXT.
*/

#ifndef ER_H
#define ER_H

#include "mpi.h"

#define ER_SUCCESS (0)
#define ER_FAILURE (1)

#define ER_DIRECTION_ENCODE  (1)
#define ER_DIRECTION_REBUILD (2)

int ER_Init(
  const char* conf_file
);

int ER_Finalize();

int ER_Create_Scheme(
  MPI_Comm comm,
  const char* failure_domain,
  int data_blocks,
  int erasure_blocks
);

int ER_Free_Scheme(int scheme_id);

/* create a named set, and specify whether it should be encoded or recovered */
int ER_Create(
  const char* name,
  int direction,
  int scheme_id /* encoding scheme to be applied */
);

/* adds file to specified set id */
int ER_Add(
  int set_id,
  const char* file
);

/* initiate encode/rebuild operation on specified set id */
int ER_Dispatch(
  int set_id
);

/* tests whether ongoing dispatch operation to finish,
 * returns 1 if done, 0 otherwise */
int ER_Test(
  int set_id
);

/* wait for ongoing dispatch operation to finish */
int ER_Wait(
  int set_id
);

/* free internal resources associated with set id */
int ER_Free(
  int set_id
);

#endif /* ER_H */
