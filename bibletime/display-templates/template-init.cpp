void CDisplayTemplateMgr::init() {
	m_templateMap[ i18n("Default") ] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?> <!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\"> <html xmlns=\"http://www.w3.org/1999/xhtml\"> <head> 	<title>#TITLE#</title> 	<meta name=\"GENERATOR\" content=\"BibleTime \" VERSION \"\" /> 	<meta name=\"AUTHOR\" content=\"BibleTime \" VERSION \"\" /> 	<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" /> 	<style type=\"text/css\"> 	/* <![CDATA[ */ * { /* To work around a KDE 3.2 bug/problem */ 	background-color:inherit;  }  body { 	margin:15px; } #content { 	background-color:white; } #content table { 	margin:0; 	padding:0; 	border-spacing:10px; 	border-collapse: collapse; 	vertical-align: top; } #content table th { 	padding: 0 0 2px 0; 	text-align: center;  	font-weight: bold; 	font-size: 115%; 	border-bottom: 1px solid #6B6B6B; }  a { 	text-decoration:none; 	font-weight:normal; 	color: #3B11AE; 	padding:2px; } a:hover { 	background-color:#F6FFF7; 	color: #AE1518; 	padding:2px; }  td.entry, div.entry { 	padding: 2px; 	vertical-align: top; } span.entry { 	padding: 2px; }  #multiple div.currententry, #multiple td.currententry, div.currententry { 	padding: 5px; 	border: 1px solid black; 	background-color: #F6FFF7; 	vertical-align: top; } #multiple td.entry + td.entry, td.entry + td.entry {  	margin-left: 2px; } #multiple span.currententry, #multiple span.currententry { 	background-color: #C0F6C8; 	vertical-align: middle; 	padding: 2px; } .entry, .currententry { 	text-align: justify; }  .versenum, .entryname { 	padding: 0 2px 0 2px; } .versenum a, .entryname a  { 	font-size: 70%; 	vertical-align: top; }  .footnotepre { 	color: gray; 	font-size: 0.9em; 	font-style: italic; } .footnote { 	color: gray; 	font-size: 0.9em; 	font-style: italic; }  .strongnumber { 	vertical-align: top; 	font-size: 50%; 	color: gray; } .morphcode { 	vertical-align: top; 	font-size: 60%; 	color: gray; } .lemma { } .strongnumber:hover, .morphcode:hover { 	color: darkRed; }  .sectiontitle { 	font-weight: bold; 	font-size: 120%; } * + .sectiontitle { 	margin-top:15px; }  .booktitle { 	font-weight: bold; 	font-size: 140%; 	font-variant: small-caps; } .foreign { } .jesuswords { 	color: #9C2323; 	font-size: 0.9em; }  .quotation { } .poetry { } .sup { 	vertical-align: super; } .sub { 	vertical-align: sub; } .right { 	text-align: right; } .center { 	text-align: center; }  .bold { } .illuminated { } .italic { } .line-through { } .normal { } .small-caps{ } .underline { }  .inscription { } .mentioned { } .name > .geographic { } .name > .holiday { } .name > .nonhuman { } .name > .person { } .name > .ritual { } .name > .divine { }  .transchange {  	font-style:italic;  } .transchange > .added { 	background-color:inherit; } .transchange > .amplified {	 } .transchange > .changed {	 } .transchange > .deleted { } .transchange > .moved { } .transchange > .tenseChange { }  #LANG_CSS# 	/* ]]> */ 	</style> </head>  <body id=\"#DISPLAY_TYPE#\"> 	<div id=\"content\" lang=\"#LANG_ABBREV#\"> 		#CONTENT# 	</div> </body> </html>";
	m_templateMap[ i18n("Export") ] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?> <!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\"> <html xmlns=\"http://www.w3.org/1999/xhtml\"> <head> 	<title>#TITLE#</title> 	<meta name=\"GENERATOR\" content=\"BibleTime \" VERSION \"\" /> 	<meta name=\"AUTHOR\" content=\"BibleTime \" VERSION \"\" /> 	<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" /> 	<style type=\"text/css\"> 	/* <![CDATA[ */ * { /* To work around a KDE 3.2 bug/problem */ 	background-color:inherit;  }  body { 	margin:15px; } #content { 	background-color:white; } #content table { 	margin:0; 	padding:0; 	border-spacing:10px; 	border-collapse: collapse; 	vertical-align: top; } #content table th { 	padding: 0 0 2px 0; 	text-align: center;  	font-weight: bold; 	font-size: 115%; 	border-bottom: 1px solid #6B6B6B; }  a { 	text-decoration:none; 	font-weight:normal; 	color: #3B11AE; 	padding:2px; } a:hover { 	background-color:#F6FFF7; 	color: #AE1518; 	padding:2px; }  td.entry, div.entry { 	padding: 2px; 	vertical-align: top; } span.entry { 	padding: 2px; }  #multiple div.currententry, #multiple td.currententry, div.currententry { 	padding: 5px; 	border: 1px solid black; 	background-color: #F6FFF7; 	vertical-align: top; } #multiple td.entry + td.entry, td.entry + td.entry {  	margin-left: 2px; } #multiple span.currententry, #multiple span.currententry { 	background-color: #C0F6C8; 	vertical-align: middle; 	padding: 2px; }  .entry, .currententry { 	text-align: justify; 	margin-bottom:15px; }  .entryname { 	padding: 0 2px 0 2px; 	display:block; 	float:none; 	clear:both; }  .footnotepre { 	color: gray; 	font-size: 0.9em; 	font-style: italic; } .footnote { 	color: gray; 	font-size: 0.9em; 	font-style: italic; }  .strongnumber { 	vertical-align: top; 	font-size: 50%; 	color: gray; } .morphcode { 	vertical-align: top; 	font-size: 60%; 	color: gray; } .lemma { } .strongnumber:hover, .morphcode:hover { 	color: darkRed; }  .sectiontitle { 	font-weight: bold; 	font-size: 120%; } * + .sectiontitle { 	margin-top:15px; }  .booktitle { 	font-weight: bold; 	font-size: 140%; 	font-variant: small-caps; } .foreign { } .jesuswords { 	color: #9C2323; 	font-size: 0.9em; }  .quotation { } .poetry { } .sup { 	vertical-align: super; } .sub { 	vertical-align: sub; } .right { 	text-align: right; } .center { 	text-align: center; }  .bold { } .illuminated { } .italic { } .line-through { } .normal { } .small-caps{ } .underline { }  .inscription { } .mentioned { } .name > .geographic { } .name > .holiday { } .name > .nonhuman { } .name > .person { } .name > .ritual { } .name > .divine { }  .transchange {  	font-style:italic;  } .transchange > .added { 	background-color:inherit; } .transchange > .amplified {	 } .transchange > .changed {	 } .transchange > .deleted { } .transchange > .moved { } .transchange > .tenseChange { }  #LANG_CSS# 	/* ]]> */ 	</style> </head>  <body id=\"#DISPLAY_TYPE#\"> 	<div id=\"content\" lang=\"#LANG_ABBREV#\"> 		#CONTENT# 	</div> </body> </html>";
	m_templateMap[ i18n("Simple ") ] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?> <!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\"> <html xmlns=\"http://www.w3.org/1999/xhtml\"> <head> 	<title>#TITLE#</title> 	<meta name=\"GENERATOR\" content=\"BibleTime \" VERSION \"\" /> 	<meta name=\"AUTHOR\" content=\"BibleTime \" VERSION \"\" /> 	<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" /> 	 	<style type=\"text/css\"> /* <![CDATA[ */ body { 	margin:5px; } #content { 	background-color:white; } #content table { 	margin:0; 	padding:0; 	border-spacing:10px; 	border-collapse: collapse; 	vertical-align: top; } #content table th { 	padding: 0 0 2px 0; 	text-align: center;  	font-weight: bold; 	font-size: 115%; }  a { 	text-decoration:none; 	font-weight:normal; 	color: blue; 	padding:2px; }  div.verse, td.verse, div.entry { 	padding: 5px; 	vertical-align: top; } span.verse, span.entry { 	padding: 5px; }  div.currentverse, td.currentverse, div.currententry { 	padding: 5px; 	vertical-align: top; 	font-weight:bold; } td.verse + td.verse, td.entry + td.entry {  	margin-left: 5px; } span.currentverse, span.currententry { 	font-weight:bold; 	vertical-align: middle; 	padding: 5px; }  .footnotepre { 	color: gray; } .footnote { 	color: gray; }  .strongnumber { 	vertical-align: top; 	font-size: 60%; 	color: blue; } .morphcode { 	vertical-align: top; 	font-size: 60%; 	color: blue; } .lemma { }  .sectiontitle { 	font-weight: bold; 	font-size: 120%; } .verse + .sectiontitle { 	margin-top:35px; 	padding-top:35px; }  .booktitle { 	font-weight: bold; 	font-size: 140%; } .foreign { } .jesuswords { 	color: red; 	font-size: 0.9em; }  .quotation { } .poetry { } .sup { 	vertical-align: super; } .sub { 	vertical-align: sub; } .right { 	text-align: right; } .center { 	text-align: center; }   #LANG_CSS# /* ]]> */ 	</style> </head>  <body> 	<div id=\"content\" lang=\"#LANG_ABBREV#\"> 		#CONTENT# 	</div> </body> </html>";

}
