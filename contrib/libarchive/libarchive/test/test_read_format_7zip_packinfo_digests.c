/*-
 * Copyright (c) 2019 Martin Matuska
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "test.h"
__FBSDID("$FreeBSD");

/* Read archive with digests in PackInfo */
DEFINE_TEST(test_read_format_7zip_packinfo_digests)
{
	struct archive_entry *ae;
	struct archive *a;
	char buff[4];
	const char *refname = "test_read_format_7zip_packinfo_digests.7z";

	extract_reference_file(refname);
	assert((a = archive_read_new()) != NULL);
	if (ARCHIVE_OK != archive_read_support_filter_xz(a)) {
		skipping("7zip:lzma decoding is not supported on this "
		"platform");
	} else {
		assertEqualIntA(a, ARCHIVE_OK,
		    archive_read_support_filter_all(a));
		assertEqualIntA(a, ARCHIVE_OK,
		    archive_read_support_format_all(a));
		assertEqualIntA(a, ARCHIVE_OK,
		    archive_read_open_filename(a, refname, 10240));

		/* Verify regular file1. */
		assertEqualIntA(a, ARCHIVE_OK,
		    archive_read_next_header(a, &ae));
		assertEqualInt((AE_IFREG | 0666), archive_entry_mode(ae));
		assertEqualString("a.txt", archive_entry_pathname(ae));
		assertEqualInt(1576808819, archive_entry_mtime(ae));
		assertEqualInt(4, archive_entry_size(ae));
		assertEqualInt(archive_entry_is_encrypted(ae), 0);
		assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);
		assertEqualInt(4, archive_read_data(a, buff, sizeof(buff)));
		assertEqualMem(buff, "aaa\n", 4);

		/* Verify regular file2. */
		assertEqualIntA(a, ARCHIVE_OK,
		    archive_read_next_header(a, &ae));
		assertEqualInt((AE_IFREG | 0666), archive_entry_mode(ae));
		assertEqualString("b.txt", archive_entry_pathname(ae));
		assertEqualInt(1576808819, archive_entry_mtime(ae));
		assertEqualInt(4, archive_entry_size(ae));
		assertEqualInt(archive_entry_is_encrypted(ae), 0);
		assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);
		assertEqualInt(4, archive_read_data(a, buff, sizeof(buff)));
		assertEqualMem(buff, "bbb\n", 4);

		assertEqualInt(2, archive_file_count(a));

		/* End of archive. */
		assertEqualIntA(a, ARCHIVE_EOF,
		    archive_read_next_header(a, &ae));

		/* Verify archive format. */
		assertEqualIntA(a, ARCHIVE_FILTER_NONE,
		    archive_filter_code(a, 0));
		assertEqualIntA(a, ARCHIVE_FORMAT_7ZIP,
		    archive_format(a));

		/* Close the archive. */
		assertEqualInt(ARCHIVE_OK, archive_read_close(a));
	}
	assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}
