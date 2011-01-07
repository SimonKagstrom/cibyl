install (PROGRAMS
  cibyl-generate-c-header
  cibyl-generate-java-wrappers
  cibyl-generate-cibar
  cibyl-peephole-optimize
  cibyl-generate-syscall-db
  cibyl-mips2java
  DESTINATION ${DESTDIR}/bin)

install (DIRECTORY
  python
  DESTINATION ${DESTDIR}/bin)
