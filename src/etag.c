#include "buffer.h"
#include "etag.h"

#if defined HAVE_STDINT_H
# include <stdint.h>
#elif defined HAVE_INTTYPES_H
# include <inttypes.h>
#endif

#include <string.h>

int etag_is_equal(buffer *etag, const char *matches) {
	if (etag && !buffer_string_is_empty(etag) && 0 == strcmp(etag->ptr, matches)) return 1;
	return 0;
}

int etag_create(buffer *etag, struct stat *st,etag_flags_t flags) {
	if (0 == flags) return 0;

	buffer_reset(etag);

	if (flags & ETAG_USE_INODE) {
		buffer_append_int(etag, st->st_ino);
		buffer_append_string_len(etag, CONST_STR_LEN("-"));
	}
	
	if (flags & ETAG_USE_SIZE) {
		buffer_append_int(etag, st->st_size);
		buffer_append_string_len(etag, CONST_STR_LEN("-"));
	}
	
	if (flags & ETAG_USE_MTIME) {
		buffer_append_int(etag, st->st_mtime);
	}

	return 0;
}

int etag_mutate(buffer *mut, buffer *etag) {
	size_t i, len;
	uint32_t h;

	len = buffer_string_length(etag);
	for (h=0, i=0; i < len; ++i) h = (h<<5)^(h>>27)^(etag->ptr[i]);

	buffer_reset(mut);
	buffer_copy_string_len(mut, CONST_STR_LEN("\""));
	buffer_append_int(mut, h);
	buffer_append_string_len(mut, CONST_STR_LEN("\""));

	return 0;
}
