#include <sys/types.h> /* size_t, ssize_t */
#include <stdarg.h> /* va_list */
#include <stddef.h> /* NULL */
#include <stdint.h> /* int64_t */
#include <kcgi.h>


enum pg {
  PG_INDEX,
  PG_CONTACT,
  PG__MAX
};

static const char *pages[PG__MAX] = {
  "index",
  "contact",
};

int main(void) {
    struct kreq r;

    if (KCGI_OK != khttp_parse(&r, NULL, 0, pages, PG__MAX, PG_CONTACT))
        return 0;

	switch(r.page) {
	case PG_INDEX:
	    khttp_head(&r, kresps[KRESP_STATUS], 
	        "%s", khttps[KHTTP_200]);
	    khttp_head(&r, kresps[KRESP_CONTENT_TYPE], 
	        "%s", kmimetypes[KMIME_TEXT_PLAIN]);
	    khttp_body(&r);
	    khttp_puts(&r, "default");
	    khttp_free(&r);
	    break;
	case PG_CONTACT:
	    khttp_head(&r, kresps[KRESP_STATUS], 
	        "%s", khttps[KHTTP_200]);
	    khttp_head(&r, kresps[KRESP_CONTENT_TYPE], 
	        "%s", kmimetypes[KMIME_TEXT_PLAIN]);
	    khttp_body(&r);
	    khttp_puts(&r, "contact");
	    khttp_free(&r);
	    break;
	default:
	    khttp_head(&r, kresps[KRESP_STATUS], 
	        "%s", khttps[KHTTP_200]);
	    khttp_head(&r, kresps[KRESP_CONTENT_TYPE], 
	        "%s", kmimetypes[KMIME_TEXT_PLAIN]);
	    khttp_body(&r);
	    khttp_puts(&r, "error");
	    khttp_free(&r);
	    break;
	}
    return 0;
}
