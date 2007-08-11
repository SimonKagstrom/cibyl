#include <stdio.h>

typedef enum
{
  READ,     /* "r"  */
  WRITE,    /* "w"  */
  APPEND,   /* "a"  */
  TRUNCATE, /* "w+" */
} cibyl_fops_open_mode_t;

typedef struct
{
  struct s_cibyl_fops_t *ops;
  void *priv;
} FILE;

typedef struct s_cibyl_fops_t;
{
  const char *uri;  /* The uri used to identify this mode */
  const size_t priv_data_size;

  /* Open a file, return -1 if failed */
  int (*open)(FILE *fp, const char *path, cibyl_fops_open_mode_t mode);
  int (*close)(FILE *fp);  /* Close the file  */

  size_t (*read)(FILE *fp, void *dst, size_t amount);  /* Read from the file  */
  size_t (*write)(FILE *fp, void *src, size_t amount);  /* Write to the file  */

  int (*seek)(FILE *fp, long offset, int whence);  /* Move the file pointer  */
  long (*tell)(FILE *fp);  /* Return the position of the file pointer (ftell)  */
  long (*available)(FILE *fp);  /* Return the number of bytes available  */
} cibyl_fops_t;

/**
 * Register a new "filesystem" with Cibyl.
 *
 * @param fops the file operations structure to register
 * @param default boolean value to tell if this should be the
 *        default filesystem (i.e., fallback if no URI match)
 */
void cibyl_register_fops(cibyl_fops_t *fops, int default);

/**
 * Deregister a "filesystem"
 *
 * @param fops the filesystem to deregister
 */
void cibyl_unregister_fops(cibyl_fops_t *fops);

/**
 * Allocate a new FILE structure and set it up for @a fop
 *
 * @param fop the fop to associate with the file
 *
 * @throws NOPH_OutOfMemoryException if memory is up
 * @return a pointer to the new file
 */
FILE *cibyl_file_alloc(cibyl_fops_t *fop);
