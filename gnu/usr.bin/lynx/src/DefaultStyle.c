/*	A real style sheet for the Character Grid browser
**
**	The dimensions are all in characters!
*/

#include <HTUtils.h>
#include <HTFont.h>
#include <HTStyle.h>

#include <LYLeaks.h>

/*	Tab arrays:
*/
PRIVATE CONST HTTabStop tabs_8[] = {
	{ 0, 8 }, {0, 16}, {0, 24}, {0, 32}, {0, 40},
	{ 0, 48 }, {0, 56}, {0, 64}, {0, 72}, {0, 80},
	{ 0, 88 }, {0, 96}, {0, 104}, {0, 112}, {0, 120},
	{ 0, 128 }, {0, 136}, {0, 144}, {0, 152}, {0, 160},
	{0, 168}, {0, 176},
	{0, 0 }		/* Terminate */
};

/* Template:
**	link to next, name, name id (enum), tag,
**	font, size, colour, superscript, anchor id,
**	indents: 1st, left, right, alignment	lineheight, descent,	tabs,
**	word wrap, free format, space: before, after, flags.
*/

PRIVATE HTStyle HTStyleNormal = {
	0,  "Normal", ST_Normal, "P",
	HT_FONT, 1, HT_BLACK,		0, 0,
	3, 3, 6, HT_LEFT,		1, 0,	tabs_8,
	YES, YES, 1, 0,			0 };

PRIVATE HTStyle HTStyleDivCenter = {
	&HTStyleNormal,  "DivCenter", ST_DivCenter, "DCENTER",
	HT_FONT, 1, HT_BLACK,		0, 0,
	3, 3, 6, HT_CENTER,		1, 0,	tabs_8,
	YES, YES, 1, 0,			0 };

PRIVATE HTStyle HTStyleDivLeft = {
	&HTStyleDivCenter,  "DivLeft", ST_DivLeft, "DLEFT",
	HT_FONT, 1, HT_BLACK,		0, 0,
	3, 3, 6, HT_LEFT,		1, 0,	tabs_8,
	YES, YES, 1, 0,			0 };

PRIVATE HTStyle HTStyleDivRight = {
	&HTStyleDivLeft,  "DivRight", ST_DivRight, "DRIGHT",
	HT_FONT, 1, HT_BLACK,		0, 0,
	3, 3, 6, HT_RIGHT,		1, 0,	tabs_8,
	YES, YES, 1, 0,			0 };

PRIVATE HTStyle HTStyleBanner = {
	&HTStyleDivRight,  "Banner", ST_Banner, "BANNER",
	HT_FONT, 1, HT_BLACK,		0, 0,
	3, 3, 6, HT_LEFT,		1, 0,	tabs_8,
	YES, YES, 1, 0,			0 };

PRIVATE HTStyle HTStyleBlockquote = {
	&HTStyleBanner,  "Blockquote", ST_Blockquote, "BLOCKQUOTE",
	HT_FONT, 1, HT_BLACK,		0, 0,
	5, 5, 7, HT_LEFT,		1, 0,	tabs_8,
	YES, YES, 1, 0,			0 };

PRIVATE HTStyle HTStyleBq = { /* HTML 3.0 BLOCKQUOTE - FM */
	&HTStyleBlockquote,  "Bq", ST_Bq, "BQ",
	HT_FONT, 1, HT_BLACK,		0, 0,
	5, 5, 7, HT_LEFT,		1, 0,	tabs_8,
	YES, YES, 1, 0,			0 };

PRIVATE HTStyle HTStyleFootnote = { /* HTML 3.0 FN - FM */
	&HTStyleBq,  "Footnote", ST_Footnote, "FN",
	HT_FONT, 1, HT_BLACK,		0, 0,
	5, 5, 7, HT_LEFT,		1, 0,	tabs_8,
	YES, YES, 1, 0,			0 };

PRIVATE HTStyle HTStyleList = {
	&HTStyleFootnote,  "List", ST_List, "UL",
	HT_FONT, 1, HT_BLACK,		0, 0,
	3, 7, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 0, 0,			0 };

PRIVATE HTStyle HTStyleList1 = {
	&HTStyleList,  "List1", ST_List1, "UL",
	HT_FONT, 1, HT_BLACK,		0, 0,
	8, 12, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 0, 0,			0 };

PRIVATE HTStyle HTStyleList2 = {
	&HTStyleList1,  "List2", ST_List2, "UL",
	HT_FONT, 1, HT_BLACK,		0, 0,
	13, 17, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 0, 0,			0 };

PRIVATE HTStyle HTStyleList3 = {
	&HTStyleList2,  "List3", ST_List3, "UL",
	HT_FONT, 1, HT_BLACK,		0, 0,
	18, 22, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 0, 0,			0 };

PRIVATE HTStyle HTStyleList4 = {
	&HTStyleList3,  "List4", ST_List4, "UL",
	HT_FONT, 1, HT_BLACK,		0, 0,
	23, 27, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 0, 0,			0 };

PRIVATE HTStyle HTStyleList5 = {
	&HTStyleList4,  "List5", ST_List5, "UL",
	HT_FONT, 1, HT_BLACK,		0, 0,
	28, 32, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 0, 0,			0 };

PRIVATE HTStyle HTStyleList6 = {
	&HTStyleList5,  "List6", ST_List6, "UL",
	HT_FONT, 1, HT_BLACK,		0, 0,
	33, 37, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 0, 0,			0 };

PRIVATE HTStyle HTStyleMenu = {
	&HTStyleList6,  "Menu", ST_Menu, "MENU",
	HT_FONT, 1, HT_BLACK,		0, 0,
	3, 7, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 0, 0,			0
};

PRIVATE HTStyle HTStyleMenu1 = {
	&HTStyleMenu,  "Menu1", ST_Menu1, "MENU",
	HT_FONT, 1, HT_BLACK,		0, 0,
	8, 12, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 0, 0,			0
};

PRIVATE HTStyle HTStyleMenu2= {
	&HTStyleMenu1,  "Menu2", ST_Menu2, "MENU",
	HT_FONT, 1, HT_BLACK,		0, 0,
	13, 17, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 0, 0,			0
};

PRIVATE HTStyle HTStyleMenu3= {
	&HTStyleMenu2,  "Menu3", ST_Menu3, "MENU",
	HT_FONT, 1, HT_BLACK,		0, 0,
	18, 22, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 0, 0,			0
};

PRIVATE HTStyle HTStyleMenu4= {
	&HTStyleMenu3,  "Menu4", ST_Menu4, "MENU",
	HT_FONT, 1, HT_BLACK,		0, 0,
	23, 27, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 0, 0,			0
};

PRIVATE HTStyle HTStyleMenu5= {
	&HTStyleMenu4,  "Menu5", ST_Menu5, "MENU",
	HT_FONT, 1, HT_BLACK,		0, 0,
	28, 33, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 0, 0,			0
};

PRIVATE HTStyle HTStyleMenu6= {
	&HTStyleMenu5,  "Menu6", ST_Menu6, "MENU",
	HT_FONT, 1, HT_BLACK,		0, 0,
	33, 38, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 0, 0,			0
};

PRIVATE HTStyle HTStyleGlossary = {
	&HTStyleMenu6,  "Glossary", ST_Glossary, "DL",
	HT_FONT, 1, HT_BLACK,		0, 0,
	3, 10, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 1, 1,			0
};

PRIVATE HTStyle HTStyleGlossary1 = {
	&HTStyleGlossary,  "Glossary1", ST_Glossary1, "DL",
	HT_FONT, 1, HT_BLACK,		0, 0,
	8, 16, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 1, 1,			0
};

PRIVATE HTStyle HTStyleGlossary2 = {
	&HTStyleGlossary1,  "Glossary2", ST_Glossary2, "DL",
	HT_FONT, 1, HT_BLACK,		0, 0,
	14, 22, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 1, 1,			0
};

PRIVATE HTStyle HTStyleGlossary3 = {
	&HTStyleGlossary2,  "Glossary3", ST_Glossary3, "DL",
	HT_FONT, 1, HT_BLACK,		0, 0,
	20, 28, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 1, 1,			0
};

PRIVATE HTStyle HTStyleGlossary4 = {
	&HTStyleGlossary3,  "Glossary4", ST_Glossary4, "DL",
	HT_FONT, 1, HT_BLACK,		0, 0,
	26, 34, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 1, 1,			0
};

PRIVATE HTStyle HTStyleGlossary5 = {
	&HTStyleGlossary4,  "Glossary5", ST_Glossary5, "DL",
	HT_FONT, 1, HT_BLACK,		0, 0,
	32, 40, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 1, 1,			0
};

PRIVATE HTStyle HTStyleGlossary6 = {
	&HTStyleGlossary5,  "Glossary6", ST_Glossary6, "DL",
	HT_FONT, 1, HT_BLACK,		0, 0,
	38, 46, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 1, 1,			0
};

PRIVATE HTStyle HTStyleGlossaryCompact = {
	&HTStyleGlossary6,  "GlossaryCompact", ST_GlossaryCompact, "DLC",
	HT_FONT, 1, HT_BLACK,		0, 0,
	3, 10, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 0, 0,			0
};

PRIVATE HTStyle HTStyleGlossaryCompact1 = {
	&HTStyleGlossaryCompact,  "GlossaryCompact1", ST_GlossaryCompact1, "DLC",
	HT_FONT, 1, HT_BLACK,		0, 0,
	8, 15, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 0, 0,			0
};

PRIVATE HTStyle HTStyleGlossaryCompact2 = {
	&HTStyleGlossaryCompact1,  "GlossaryCompact2", ST_GlossaryCompact2, "DLC",
	HT_FONT, 1, HT_BLACK,		0, 0,
	13, 20, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 0, 0,			0
};

PRIVATE HTStyle HTStyleGlossaryCompact3 = {
	&HTStyleGlossaryCompact2,  "GlossaryCompact3", ST_GlossaryCompact3, "DLC",
	HT_FONT, 1, HT_BLACK,		0, 0,
	18, 25, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 0, 0,			0
};

PRIVATE HTStyle HTStyleGlossaryCompact4 = {
	&HTStyleGlossaryCompact3,  "GlossaryCompact4", ST_GlossaryCompact4, "DLC",
	HT_FONT, 1, HT_BLACK,		0, 0,
	23, 30, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 0, 0,			0
};

PRIVATE HTStyle HTStyleGlossaryCompact5 = {
	&HTStyleGlossaryCompact4,  "GlossaryCompact5", ST_GlossaryCompact5, "DLC",
	HT_FONT, 1, HT_BLACK,		0, 0,
	28, 35, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 0, 0,			0
};

PRIVATE HTStyle HTStyleGlossaryCompact6 = {
	&HTStyleGlossaryCompact5,  "GlossaryCompact6", ST_GlossaryCompact6, "DLC",
	HT_FONT, 1, HT_BLACK,		0, 0,
	33, 40, 6, HT_LEFT,		1, 0,	0,
	YES, YES, 0, 0,			0
};

PRIVATE HTStyle HTStyleExample = {
	&HTStyleGlossaryCompact6,  "Example", ST_Example, "XMP",
	HT_FONT, 1, HT_BLACK,		0, 0,
	0, 0, 0, HT_LEFT,		1, 0,	tabs_8,
	NO, NO, 0, 0,			0
};

PRIVATE HTStyle HTStylePreformatted = {
	&HTStyleExample,	"Preformatted", ST_Preformatted, "PRE",
	HT_FONT, 1, HT_BLACK,		0, 0,
	0, 0, 0, HT_LEFT,		1, 0,	tabs_8,
	NO, NO, 0, 0,			0
};

PRIVATE HTStyle HTStyleListing = {
	&HTStylePreformatted,  "Listing", ST_Listing, "LISTING",
	HT_FONT, 1, HT_BLACK,		0, 0,
	0, 0, 0, HT_LEFT,		1, 0,	tabs_8,
	NO, NO, 0, 0,			0 };

PRIVATE HTStyle HTStyleAddress = {
	&HTStyleListing,  "Address", ST_Address, "ADDRESS",
	HT_FONT, 1, HT_BLACK,		0, 0,
	4, 4, 7, HT_LEFT,		1, 0,	tabs_8,
	YES, YES, 2, 0,			0 };

PRIVATE HTStyle HTStyleNote = { /* HTML 3.0 NOTE - FM */
	&HTStyleAddress,  "Note", ST_Note, "NOTE",
	HT_FONT, 1, HT_BLACK,		0, 0,
	5, 5, 7, HT_LEFT,		1, 0,	tabs_8,
	YES, YES, 1, 0,			0 };

PRIVATE HTStyle HTStyleHeading1 = {
	&HTStyleNote,  "Heading1", ST_Heading1, "H1",
	HT_FONT+HT_BOLD, 1, HT_BLACK,	0, 0,
	0, 0, 0, HT_CENTER,		1, 0,	0,
	YES, YES, 1, 1,			0 };

PRIVATE HTStyle HTStyleHeading2 = {
	&HTStyleHeading1,  "Heading2", ST_Heading2, "H2",
	HT_FONT+HT_BOLD, 1, HT_BLACK,	0, 0,
	0, 0, 0, HT_LEFT,		1, 0,	0,
	YES, YES, 1, 1,			0 };

PRIVATE HTStyle HTStyleHeading3 = {
	&HTStyleHeading2,  "Heading3", ST_Heading3, "H3",
	HT_FONT+HT_BOLD, 1, HT_BLACK,	0, 0,
	2, 2, 0, HT_LEFT,		1, 0,	0,
	YES, YES, 1, 0,			0 };

PRIVATE HTStyle HTStyleHeading4 = {
	&HTStyleHeading3,  "Heading4", ST_Heading4, "H4",
	HT_FONT+HT_BOLD, 1, HT_BLACK,	0, 0,
	4, 4, 0, HT_LEFT,		1, 0,	0,
	YES, YES, 1, 0,			0 };

PRIVATE HTStyle HTStyleHeading5 = {
	&HTStyleHeading4,  "Heading5", ST_Heading5, "H5",
	HT_FONT+HT_BOLD, 1, HT_BLACK,	0, 0,
	6, 6, 0, HT_LEFT,		1, 0,	0,
	YES, YES, 1, 0,			0 };

PRIVATE HTStyle HTStyleHeading6 = {
	&HTStyleHeading5,  "Heading6", ST_Heading6, "H6",
	HT_FONT+HT_BOLD, 1, HT_BLACK,	0, 0,
	8, 8, 0, HT_LEFT,		1, 0,	0,
	YES, YES, 1, 0,			0 };

PRIVATE HTStyle HTStyleHeadingCenter = {
	&HTStyleHeading6,  "HeadingCenter", ST_HeadingCenter, "HCENTER",
	HT_FONT+HT_BOLD, 1, HT_BLACK,	0, 0,
	0, 0, 3, HT_CENTER,		1, 0,	tabs_8,
	YES, YES, 1, 0,			0 };

PRIVATE HTStyle HTStyleHeadingLeft = {
	&HTStyleHeadingCenter,  "HeadingLeft", ST_HeadingLeft, "HLEFT",
	HT_FONT+HT_BOLD, 1, HT_BLACK,	0, 0,
	0, 0, 3, HT_LEFT,		1, 0,	tabs_8,
	YES, YES, 1, 0,			0 };

PRIVATE HTStyle HTStyleHeadingRight = {
	&HTStyleHeadingLeft,  "HeadingRight", ST_HeadingRight, "HRIGHT",
	HT_FONT+HT_BOLD, 1, HT_BLACK,	0, 0,
	0, 0, 3, HT_RIGHT,		1, 0,	tabs_8,
	YES, YES, 1, 0,			0 };

/* Style sheet points to the last in the list:
*/
PRIVATE HTStyleSheet sheet = { "default.style",
				&HTStyleHeadingRight }; /* sheet */

PRIVATE HTStyle * st_array[ST_HeadingRight + 1] = { NULL };

PRIVATE HTStyleSheet *result = NULL;

#ifdef LY_FIND_LEAKS
PRIVATE void FreeDefaultStyle NOARGS
{
    HTStyle * style;
    while((style=result->styles)!=0) {
	result->styles = style->next;
	FREE(style);
    }
    FREE(result);
}
#endif /* LY_FIND_LEAKS */

PUBLIC HTStyleSheet * DefaultStyle ARGS1 (HTStyle ***, result_array)
{
    HTStyle *p, *q;

    /*
     * The first time we're called, allocate a copy of the 'sheet' linked
     * list.  Thereafter, simply copy the data from 'sheet' into our copy
     * (preserving the copy's linked-list pointers).  We do this to reset the
     * parameters of a style that might be altered while processing a page.
     */
    if (result == 0) {	/* allocate & copy */
	result = HTStyleSheetNew ();
	*result = sheet;
	result->styles = 0;
#ifdef LY_FIND_LEAKS
	atexit(FreeDefaultStyle);
#endif
	for (p = sheet.styles; p != 0; p = p->next) {
	    q = HTStyleNew ();
	    *q = *p;
	    st_array[q->id] = q;
	    q->next = result->styles;
	    result->styles = q;
	}
    } else {		/* recopy the data */
	for (q = result->styles, p = sheet.styles;
		p != 0 && q != 0;
		p = p->next, q = q->next) {
	    HTStyle *r = q->next;
	    *q = *p;
	    st_array[q->id] = q;
	    q->next = r;
	}
    }
    *result_array = st_array;
    return result;
}
