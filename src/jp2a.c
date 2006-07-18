/*
 * Copyright (C) 2006 Christian Stigen Larsen, http://csl.sublevel3.org
 * Distributed under the GNU General Public License (GPL) v2 or later.
 *
 * Project homepage on http://jp2a.sf.net
 *
 * $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdio.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "jp2a.h"
#include "options.h"

#ifdef WIN32
#ifdef FEAT_CURL
#include <io.h>
#define close _close
#endif
#endif

int main(int argc, char** argv) {
	parse_options(argc, argv);

	FILE *fout = stdout;

	if ( strcmp(fileout, "-") ) {
		if ( (fout = fopen(fileout, "wb")) == NULL ) {
			fprintf(stderr, "Could not open '%s' for writing.\n", fileout);
			return 1;
		}
	}

	int n;
	for ( n=1; n<argc; ++n ) {

		// skip options
		if ( argv[n][0]=='-' && argv[n][1] )
			continue;

		// read from stdin
		if ( argv[n][0]=='-' && !argv[n][1] ) {
			decompress(stdin, fout);
			continue;
		}

		#ifdef FEAT_CURL
		if ( is_url(argv[n]) ) {

			if ( verbose )
				fprintf(stderr, "URL: %s\n", argv[n]);

			int fd = curl_download(argv[n], debug);

			FILE *fr = fdopen(fd, "rb");

			if ( !fr ) {
				fputs("Could not fdopen read pipe\n", stderr);
				return 1;
			}

			decompress(fr, fout);
			fclose(fr);
			close(fd);
			
			continue;
		}
		#endif

		// read files
		FILE *fp = fopen(argv[n], "rb");

		if ( fp ) {
			if ( verbose )
				fprintf(stderr, "File: %s\n", argv[n]);

			decompress(fp, fout);
			fclose(fp);

			continue;

		} else {
			fprintf(stderr, "Can't open %s\n", argv[n]);
			return 1;
		}
	}

	if ( fout != stdout )
		fclose(fout);

	return 0;
}
