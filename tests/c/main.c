#ifndef HOST
#include <javax/microedition/lcdui.h>
#include <javax/microedition/lcdui/game.h>
#include <java/lang.h>
#endif

#include <stdio.h>
#include "console.h"
#include <stdarg.h>

int test_output(const char *fmt, ...)
{
    char outbuf[255];
    va_list ap;
    int r;

    if ( fmt == NULL )
	   return 0;

    /*
     * Print into buffer.
     */
    va_start(ap, fmt);
    r = vsnprintf(outbuf, sizeof(outbuf), fmt, ap);
    va_end(ap);

    console_push(outbuf);
    puts(outbuf);
    console_redraw();

    return r;
}

void test_run_all_tests(int run_fileops);

int do_output = 0;
extern char *fs_root;
extern int astar_main(int argc, char **argv);
int main(int argc, char **argv)
{
  int i;

  console_init();
  do_output = 1;
#ifndef HOST
  for (i = 0; i < 8; i++)
    {
      test_run_all_tests(0);
      NOPH_Thread_sleep(700);
    }
#endif
  test_run_all_tests(1);

  astar_main(0, NULL);

#ifndef HOST
  test_output("Please send %s/cibyl-tests.log\n", fs_root);
  test_output("to simon.kagstrom@gmail.com", fs_root);
  test_output("Version " __DATE__ ", press key to exit");
  console_finalize();
  while( !NOPH_GameCanvas_getKeyStates(NOPH_GameCanvas_get()) )
    NOPH_Thread_sleep(100);
#endif

  return 0;
}
