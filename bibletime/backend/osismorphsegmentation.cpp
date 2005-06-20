/******************************************************************************
 *
 * osisfootnotes -	SWFilter descendant to hide or show footnotes
 *			in an OSIS module.
 */


#include "osismorphsegmentation.h"

#include <stdlib.h>
#include <swmodule.h>
#include <swbuf.h>
#include <versekey.h>
#include <utilxml.h>
#include <utilstr.h>

const char oName[] = "Morph segmentation";
const char oTip[] = "Toggles morph segmentation On and Off if they exist";

const SWBuf choices[3] = {"Off", "On", ""};
const StringList oValues(&choices[0], &choices[2]);

namespace Filters {

OSISMorphSegmentation::OSISMorphSegmentation() : sword::SWOptionFilter(oName, oTip, &oValues) {
	setOptionValue("Off");	
}


OSISMorphSegmentation::~OSISMorphSegmentation() {
}


char OSISMorphSegmentation::processText(SWBuf &text, const SWKey */*key*/, const SWModule */*module*/) {
	SWBuf token;
	bool intoken    = false;
	bool hide       = false;

	SWBuf orig = text;
	const char *from = orig.c_str();
	
	XMLTag tag;

	for (text = ""; *from; ++from) {
		if (*from == '<') {
			intoken = true;
			token = "";
			continue;
		}

		if (*from == '>') {	// process tokens
			intoken = false;
			if (!strncmp(token.c_str(), "seg", 3) || !strncmp(token.c_str(), "/seg", 4)) {
				tag = token;
				
				if (!tag.isEndTag()) { //<seg type="morph">
					if (tag.getAttribute("type") && !strcmp("morph", tag.getAttribute("type"))) {
						hide = !option; //only hide if option is Off
					}
				}

				if (!hide) { //hides start and end tags as long as hide is set
					text.append('<');
					text.append(token);
					text.append('>');
				}
				
				if (tag.isEndTag()) { //</seg>
					hide = false;
				}
			} //end of seg tag handling

 			continue;
		} //end of intoken part

		if (intoken) { //copy token
			token.append(*from);
		}
		else { //copy text which is not inside of a tag
			text.append(*from);
		}
	}

	return 0;
}

}