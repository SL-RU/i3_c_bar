/*
 * See LICENSE file for license details.
 */
#include "skb.h"

#define XKB_CTRLS_MASK (XkbAllControlsMask & ~(XkbInternalModsMask | XkbIgnoreLockModsMask))

#define MAXGROUPLENGTH 256
#define OUTPUT_LENGTH 3

void
eprint(const char *errstr, ...)
{
    exit(EXIT_FAILURE);
}

int
get_gr_num(Display *dpy, XkbDescPtr kb) {
    int rv;

    if (XkbGetControls(dpy, XKB_CTRLS_MASK, kb) != Success)
	eprint("skb: XkbGetControls() failed.\n");
    rv = kb->ctrls->num_groups;
    XkbFreeControls(kb, XKB_CTRLS_MASK, 0);
    return rv;
}

void
get_gr_names(Display *dpy, XkbDescPtr kb, int ngroups, char **groups) {
    char *name = NULL;
    int i;

    if (XkbGetNames(dpy, XkbGroupNamesMask, kb) != Success)
        eprint("skb: XkbGetNames() failed");
  
    for (i = 0; i < ngroups; i++) {
        if (kb->names->groups[i]) {
            if ((name = XGetAtomName(dpy, kb->names->groups[i])))
		snprintf(groups[i], OUTPUT_LENGTH+1, name);
            else
		eprint("skb: XGetAtomName() failed\n");
        }
    }
    XkbFreeNames(kb, XkbGroupNamesMask, 0);
}

void
get_active_gr(Display *dpy, int *active) {
    XkbStateRec state;

    if (XkbGetState(dpy, XkbUseCoreKbd, &state) != Success)
	eprint("skb: XkbGetState() failed\n");
    *active = state.group;        
}

Display *dpy;
XkbDescPtr kb;
XkbEvent ev;
int ngroups = 0;
char **groups;
int active = 0;
int old = -1;
int i;

int skb_init (){
    if(!(dpy = XOpenDisplay(0)))
	eprint("skb: cannot open display\n");

    if (!(kb = XkbAllocKeyboard()))
	eprint("skb: XkbAllocKeyboard()\n");

    ngroups = get_gr_num(dpy, kb);

    groups = malloc(sizeof(char*)*ngroups);
    for (i = 0; i < ngroups; i++)
	    groups[i] = malloc(MAXGROUPLENGTH); 
    
    get_gr_names(dpy, kb, ngroups, groups);

    XkbSelectEvents(dpy, XkbUseCoreKbd, XkbAllEventsMask, XkbAllEventsMask);
    
    return 0;
}

char* skb_get()
{
    get_active_gr(dpy, &active);
    return groups[active];
}
