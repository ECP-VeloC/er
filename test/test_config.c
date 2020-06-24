#include <stdio.h>
#include <stdlib.h>
#include "er.h"
#include "er_util.h"

#include "kvtree.h"
#include "kvtree_util.h"

/* helper function to check for known options */
void check_known_options(const kvtree* configured_values,
                         const char* known_options[])
{
  /* report all unknown options (typos?) */
  const kvtree_elem* elem;
  for (elem = kvtree_elem_first(configured_values);
       elem != NULL;
       elem = kvtree_elem_next(elem))
  {
    const char* key = kvtree_elem_key(elem);

    /* must be only one level deep, ie plain kev = value */
    const kvtree* elem_hash = kvtree_elem_hash(elem);
    if (kvtree_size(elem_hash) != 1) {
      printf("Element %s has unexpected number of values: %d", key,
           kvtree_size(elem_hash));
      exit(EXIT_FAILURE);
    }

    const kvtree* kvtree_first_elem_hash =
      kvtree_elem_hash(kvtree_elem_first(elem_hash));
    if (kvtree_size(kvtree_first_elem_hash) != 0) {
      printf("Element %s is not a pure value", key);
      exit(EXIT_FAILURE);
    }

    /* check against known options */
    const char** opt;
    int found = 0;
    for (opt = known_options; *opt != NULL; opt++) {
      if (strcmp(*opt, key) == 0) {
        found = 1;
        break;
      }
    }
    if (! found) {
      printf("Unknown configuration parameter '%s' with value '%s'\n",
        kvtree_elem_key(elem),
        kvtree_elem_key(kvtree_elem_first(kvtree_elem_hash(elem)))
      );
      exit(EXIT_FAILURE);
    }
  }
}

/* helper function to check option values in kvtree against expected values */
void check_options(const int exp_debug, const int exp_set_size,
                   const int exp_mpi_buf_size)
{
  kvtree* config = ER_Config(NULL);
  if (config == NULL) {
    printf("ER_Config failed\n");
    exit(EXIT_FAILURE);
  }

  int cfg_debug;
  if (kvtree_util_get_int(config, ER_KEY_CONFIG_DEBUG, &cfg_debug) !=
    KVTREE_SUCCESS)
  {
    printf("Could not get %s from ER_Config\n",
           ER_KEY_CONFIG_DEBUG);
    exit(EXIT_FAILURE);
  }
  if (cfg_debug != exp_debug) {
    printf("ER_Config returned unexpected value %d for %s. Expected %d.\n",
           cfg_debug, ER_KEY_CONFIG_DEBUG,
           exp_debug);
    exit(EXIT_FAILURE);
  }

  int cfg_set_size;
  if (kvtree_util_get_int(config, ER_KEY_CONFIG_SET_SIZE, &cfg_set_size) !=
    KVTREE_SUCCESS)
  {
    printf("Could not get %s from ER_Config\n",
           ER_KEY_CONFIG_SET_SIZE);
    exit(EXIT_FAILURE);
  }
  if (cfg_set_size != exp_set_size) {
    printf("ER_Config returned unexpected value %d for %s. Expected %d.\n",
           cfg_set_size, ER_KEY_CONFIG_SET_SIZE,
           exp_set_size);
    exit(EXIT_FAILURE);
  }

  int cfg_mpi_buf_size;
  if (kvtree_util_get_int(config, ER_KEY_CONFIG_MPI_BUF_SIZE,
                          &cfg_mpi_buf_size) != KVTREE_SUCCESS)
  {
    printf("Could not get %s from ER_Config\n",
           ER_KEY_CONFIG_MPI_BUF_SIZE);
    exit(EXIT_FAILURE);
  }
  if (cfg_mpi_buf_size != exp_mpi_buf_size) {
    printf("ER_Config returned unexpected value %d for %s. Expected %d.\n",
           cfg_mpi_buf_size, ER_KEY_CONFIG_MPI_BUF_SIZE,
           exp_mpi_buf_size);
    exit(EXIT_FAILURE);
  }

  static const char* known_options[] = {
    ER_KEY_CONFIG_DEBUG,
    ER_KEY_CONFIG_SET_SIZE,
    ER_KEY_CONFIG_MPI_BUF_SIZE,
    /* ER_KEY_CONFIG_CRC_ON_COPY, not supported */
    NULL
  };
  check_known_options(config, known_options);

  kvtree_delete(&config);
}

int
main(int argc, char *argv[]) {
  int rc;
  char *conf_file = NULL;
  kvtree* ER_Config_values = kvtree_new();

  MPI_Init(&argc, &argv);

  rc = ER_Init(conf_file);
  if (rc != ER_SUCCESS) {
    printf("ER_Init() failed (error %d)\n", rc);
    return EXIT_FAILURE;
  }

  int old_er_debug = er_debug;
  int old_er_set_size = er_set_size;
  int old_er_mpi_buf_size = er_mpi_buf_size;

  int new_er_debug = !old_er_debug;
  int new_er_set_size = old_er_set_size + 1;
  int new_er_mpi_buf_size = old_er_mpi_buf_size + 1;

  check_options(old_er_debug, old_er_set_size, old_er_mpi_buf_size);

  /* check ER configuration settings */
  rc = kvtree_util_set_int(ER_Config_values, ER_KEY_CONFIG_DEBUG,
                           new_er_debug);
  if (rc != KVTREE_SUCCESS) {
    printf("kvtree_util_set_int failed (error %d)\n", rc);
    return EXIT_FAILURE;
  }
  rc = kvtree_util_set_int(ER_Config_values, ER_KEY_CONFIG_SET_SIZE,
                           new_er_set_size);
  if (rc != KVTREE_SUCCESS) {
    printf("kvtree_util_set_int failed (error %d)\n", rc);
    return EXIT_FAILURE;
  }

  printf("Configuring ER (first set of options)...\n");
  if (ER_Config(ER_Config_values) == NULL) {
    printf("ER_Config() failed\n");
    return EXIT_FAILURE;
  }

  /* check options we just set */

  if (er_debug != new_er_debug) {
    printf("ER_Config() failed to set %s: %d != %d\n",
           ER_KEY_CONFIG_DEBUG, er_debug, new_er_debug);
    return EXIT_FAILURE;
  }

  if (er_set_size != new_er_set_size) {
    printf("ER_Config() failed to set %s: %d != %d\n",
           ER_KEY_CONFIG_SET_SIZE, er_set_size, old_er_set_size);
    return EXIT_FAILURE;
  }

  check_options(new_er_debug, new_er_set_size, old_er_mpi_buf_size);

  /* configure remainder of options */
  kvtree_delete(&ER_Config_values);
  ER_Config_values = kvtree_new();

  rc = kvtree_util_set_int(ER_Config_values, ER_KEY_CONFIG_MPI_BUF_SIZE,
                           new_er_mpi_buf_size);
  if (rc != KVTREE_SUCCESS) {
    printf("kvtree_util_set_int failed (error %d)\n", rc);
    return EXIT_FAILURE;
  }

  printf("Configuring ER (second set of options)...\n");
  if (ER_Config(ER_Config_values) == NULL) {
    printf("ER_Config() failed\n");
    return EXIT_FAILURE;
  }

  /* check all values once more */

  if (er_debug != new_er_debug) {
    printf("ER_Config() failed to set %s: %d != %d\n",
           ER_KEY_CONFIG_DEBUG, er_debug, new_er_debug);
    return EXIT_FAILURE;
  }

  if (er_set_size != new_er_set_size) {
    printf("ER_Config() failed to set %s: %d != %d\n",
           ER_KEY_CONFIG_SET_SIZE, er_set_size, old_er_set_size);
    return EXIT_FAILURE;
  }

  if (er_mpi_buf_size != new_er_mpi_buf_size) {
    printf("ER_Config() failed to set %s: %d != %d\n",
           ER_KEY_CONFIG_MPI_BUF_SIZE, er_mpi_buf_size,
           old_er_mpi_buf_size);
    return EXIT_FAILURE;
  }

  check_options(new_er_debug, new_er_set_size, new_er_mpi_buf_size);

  rc = ER_Finalize();
  if (rc != ER_SUCCESS) {
    printf("ER_Finalize() failed (error %d)\n", rc);
    return EXIT_FAILURE;
  }

  MPI_Finalize();

  return ER_SUCCESS;
}
