/* yes - output a string repeatedly until killed
   Copyright (C) 1991 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* David MacKenzie <djm@ai.mit.edu> */

#include <stdio.h>

void
main (argc, argv)
     int argc;
     char **argv;
{
  int i;

  if (argc == 1)
    while (1)
      puts ("y");

  while (1)
    for (i = 1; i < argc; i++)
      {
	fputs (argv[i], stdout);
	putchar (i == argc - 1 ? '\n' : ' ');
      }
}
