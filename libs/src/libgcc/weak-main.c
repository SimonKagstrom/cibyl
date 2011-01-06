/*
 * Define a weak main symbol which will be overridden by the real main.
 *
 * The purpose of this is for crt0.o to have something to call for
 * pure-Java-libraries.
 */
void __attribute__((weak)) main(void)
{
}
