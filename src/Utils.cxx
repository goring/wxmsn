/*  Copyright(c) 2005 Julien "Trapamoosch" ENCHE
                                   Neil "Superna" ARMSTRONG
    This file is part of wxMSN.

    wxMSN is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    wxMSN is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with wxMSN; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


// Includes

#include "Utils.h"
StringHash	IndexedColorHash;
StringHash	SmileyHash;
StringHash	HtmlHash;

// Defines

#define SPACE nsUtils

const wxChar * status_text[8] = 
    { wxT("NLN"), wxT("BSY"), wxT("IDL"),
      wxT("BRB"), wxT("AWY"), wxT("PHN"),
      wxT("LUN"), wxT("HDN") };

ONLINE_STATUS SPACE::GetNumStatus(wxString & Str)
{
    for(size_t i = 0 ; i < 8 ; ++i)
        if(Str.CmpNoCase(status_text[i]) == 0)
            return wx_static_cast(ONLINE_STATUS, i);
    return wx_static_cast(ONLINE_STATUS, 8);
}

wxString SPACE::GetTxtStatus(ONLINE_STATUS status)
{
	if(status < 8)
		return wxString(status_text[status]);
	return wxString(status_text[0]);
}

// Implementation

wxString SPACE::BuildHtmlFromText(const wxString& Source, bool ParseEmoticons, bool ParseMSNPlusCodes, bool ParseURL, bool CleanHtml)
{
	wxString Result;
	wxString SourceCopy(Source);
	
	// Fill the remplacement HashMap
	// TODO : should be done in an Init type function or something else
	IndexedColorHash[wxT("·$68")] = wxT("#000000");
	IndexedColorHash[wxT("·$67")] = wxT("#666666");
	IndexedColorHash[wxT("·$66")] = wxT("#666666");
	IndexedColorHash[wxT("·$65")] = wxT("#009900");
	IndexedColorHash[wxT("·$64")] = wxT("#000066");
	IndexedColorHash[wxT("·$63")] = wxT("#cc0066");
	IndexedColorHash[wxT("·$62")] = wxT("#cc0000");
	IndexedColorHash[wxT("·$61")] = wxT("#996633");
	IndexedColorHash[wxT("·$60")] = wxT("#666600");
	IndexedColorHash[wxT("·$59")] = wxT("#00cc33");
	IndexedColorHash[wxT("·$58")] = wxT("#00cc33");
	IndexedColorHash[wxT("·$57")] = wxT("#003399");
	IndexedColorHash[wxT("·$56")] = wxT("#990099");
	IndexedColorHash[wxT("·$55")] = wxT("#6633cc");
	IndexedColorHash[wxT("·$54")] = wxT("#993333");
	IndexedColorHash[wxT("·$53")] = wxT("#000000");
	IndexedColorHash[wxT("·$52")] = wxT("#333333");
	IndexedColorHash[wxT("·$51")] = wxT("#666633");
	IndexedColorHash[wxT("·$50")] = wxT("#009900");
	IndexedColorHash[wxT("·$49")] = wxT("#009999");
	IndexedColorHash[wxT("·$48")] = wxT("#3399cc");
	IndexedColorHash[wxT("·$47")] = wxT("#ff00ff");
	IndexedColorHash[wxT("·$46")] = wxT("#ff0000");
	IndexedColorHash[wxT("·$45")] = wxT("#ffcc00");
	IndexedColorHash[wxT("·$44")] = wxT("#666666");
	IndexedColorHash[wxT("·$43")] = wxT("#999999");
	IndexedColorHash[wxT("·$42")] = wxT("#ffff00");
	IndexedColorHash[wxT("·$41")] = wxT("#33ff66");
	IndexedColorHash[wxT("·$40")] = wxT("#66ffff"); 
	IndexedColorHash[wxT("·$39")] = wxT("#003399");
	IndexedColorHash[wxT("·$38")] = wxT("#cc33cc");
	IndexedColorHash[wxT("·$37")] = wxT("#ff0000");
	IndexedColorHash[wxT("·$36")] = wxT("#ff9900");
	IndexedColorHash[wxT("·$35")] = wxT("#999999");
	IndexedColorHash[wxT("·$34")] = wxT("#999999");
	IndexedColorHash[wxT("·$33")] = wxT("#ffff00");
	IndexedColorHash[wxT("·$32")] = wxT("#00ff99");
	IndexedColorHash[wxT("·$31")] = wxT("#00ffff");
	IndexedColorHash[wxT("·$30")] = wxT("#0099ff");
	IndexedColorHash[wxT("·$29")] = wxT("#ff66cc");
	IndexedColorHash[wxT("·$28")] = wxT("#ff3399");
	IndexedColorHash[wxT("·$27")] = wxT("#ffcc33");
	IndexedColorHash[wxT("·$26")] = wxT("#999999");
	IndexedColorHash[wxT("·$25")] = wxT("#cccccc");
	IndexedColorHash[wxT("·$24")] = wxT("#ffff00");
	IndexedColorHash[wxT("·$23")] = wxT("#66ff66");
	IndexedColorHash[wxT("·$22")] = wxT("#66ff99");
	IndexedColorHash[wxT("·$21")] = wxT("#9966ff");
	IndexedColorHash[wxT("·$20")] = wxT("#ffcc99");
	IndexedColorHash[wxT("·$19")] = wxT("#ff6699");
	IndexedColorHash[wxT("·$18")] = wxT("#ff9999");
	IndexedColorHash[wxT("·$17")] = wxT("#cccccc");
	IndexedColorHash[wxT("·$16")] = wxT("#cccccc");
	IndexedColorHash[wxT("·$15")] = wxT("#999999");
	IndexedColorHash[wxT("·$14")] = wxT("#666666");
	IndexedColorHash[wxT("·$13")] = wxT("#ff00ff");
	IndexedColorHash[wxT("·$12")] = wxT("#0066ff");
	IndexedColorHash[wxT("·$11")] = wxT("#66ffff");
	IndexedColorHash[wxT("·$10")] = wxT("#00ccff");
	IndexedColorHash[wxT("·$9")] = wxT("#33ff00");
	IndexedColorHash[wxT("·$8")] = wxT("#ffff00");
	IndexedColorHash[wxT("·$7")] = wxT("#ff9900");
	IndexedColorHash[wxT("·$6")] = wxT("#6633cc");
	IndexedColorHash[wxT("·$5")] = wxT("#993333");
	IndexedColorHash[wxT("·$4")] = wxT("#ff0000");
	IndexedColorHash[wxT("·$3")] = wxT("#00cc66");
	IndexedColorHash[wxT("·$2")] = wxT("#0000ff");
	IndexedColorHash[wxT("·$1")] = wxT("#333333");
	IndexedColorHash[wxT("·$0")] = wxT("#ffffff");
	
	wxString ImagePath = wxPathOnly(wxTheApp->argv[0]) + wxT("/");
	SmileyHash[wxT(":-)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile001.gif' align='center'>");
	SmileyHash[wxT(":)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile001.gif' align='center'>");
	SmileyHash[wxT(":-D")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile002.gif' align='center'>");
	SmileyHash[wxT(":D")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile002.gif' align='center'>");
	SmileyHash[wxT(":-d")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile002.gif' align='center'>");
	SmileyHash[wxT(":>")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile002.gif' align='center'>");
	SmileyHash[wxT(":-O")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile008.gif' align='center'>");
	SmileyHash[wxT(":o")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile008.gif' align='center'>");
	SmileyHash[wxT(":-P")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile007.gif' align='center'>");
	SmileyHash[wxT(":p")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile007.gif' align='center'>");
	SmileyHash[wxT(";-)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile009.gif' align='center'>");
	SmileyHash[wxT(";)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile009.gif' align='center'>");
	SmileyHash[wxT(":-(")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile003.gif' align='center'>");
	SmileyHash[wxT(":(")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile003.gif' align='center'>");
	SmileyHash[wxT(":<")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile003.gif' align='center'>");
	SmileyHash[wxT("8o|")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile011.gif' align='center'>");
	SmileyHash[wxT(":-@")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile010.gif' align='center'>");
	SmileyHash[wxT(":@")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile010.gif' align='center'>");
	SmileyHash[wxT("8-)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile027.gif' align='center'>");
	SmileyHash[wxT(":'(")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile040.gif' align='center'>");
	SmileyHash[wxT(":-S")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile005.gif' align='center'>");
	SmileyHash[wxT(":-s")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile005.gif' align='center'>");
	SmileyHash[wxT(":S")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile005.gif' align='center'>");
	SmileyHash[wxT(":s")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile005.gif' align='center'>");
	SmileyHash[wxT(":-$")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile004.gif' align='center'>");
	SmileyHash[wxT(":-|")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile012.gif' align='center'>");
	SmileyHash[wxT(":|")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile012.gif' align='center'>");
	SmileyHash[wxT(":-*")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile055.gif' align='center'>");
	SmileyHash[wxT(":-#")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile056.gif' align='center'>");
	SmileyHash[wxT("(H)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile006.gif' align='center'>");
	SmileyHash[wxT("(h)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile006.gif' align='center'>");
	SmileyHash[wxT("<:o)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile041.gif' align='center'>");
	SmileyHash[wxT("(A)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/angel.gif' align='center'>");
	SmileyHash[wxT("(a)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/angel.gif' align='center'>");
	SmileyHash[wxT("+o(")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/%2Bo.gif' align='center'>");
	SmileyHash[wxT("(brb)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile043.gif' align='center'>");
	SmileyHash[wxT("(6)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile039.gif' align='center'>");
	SmileyHash[wxT("(Y)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile049.gif' align='center'>");
	SmileyHash[wxT("(y)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile049.gif' align='center'>");
	SmileyHash[wxT("(N)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile050.gif' align='center'>");
	SmileyHash[wxT("(n)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile050.gif' align='center'>");
	SmileyHash[wxT("(X)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/grl.gif' align='center'>");
	SmileyHash[wxT("(x)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/grl.gif' align='center'>");
	SmileyHash[wxT("(Z)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/boy.gif' align='center'>");
	SmileyHash[wxT("(z)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/boy.gif' align='center'>");
	SmileyHash[wxT("(L)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile015.gif' align='center'>");
	SmileyHash[wxT("(l)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile015.gif' align='center'>");
	SmileyHash[wxT("(U)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile016.gif' align='center'>");
	SmileyHash[wxT("(u)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile016.gif' align='center'>");
	SmileyHash[wxT("(K)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile020.gif' align='center'>");
	SmileyHash[wxT("(k)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile020.gif' align='center'>");
	SmileyHash[wxT("(P)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/pic.gif' align='center'>");
	SmileyHash[wxT("(p)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/pic.gif' align='center'>");
	SmileyHash[wxT("(G)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/gif>t.gif' align='center'>");
	SmileyHash[wxT("(g)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/gif>t.gif' align='center'>");
	SmileyHash[wxT("(%)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile037.gif' align='center'>");
	SmileyHash[wxT("(F)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile019.gif' align='center'>");
	SmileyHash[wxT("(f)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile019.gif' align='center'>");
	SmileyHash[wxT("(W)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile018.gif' align='center'>");
	SmileyHash[wxT("(w)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile018.gif' align='center'>");
	SmileyHash[wxT("(D)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile036.gif' align='center'>");
	SmileyHash[wxT("(d)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile036.gif' align='center'>");
	SmileyHash[wxT("(B)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile035.gif' align='center'>");
	SmileyHash[wxT("(b)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile035.gif' align='center'>");
	SmileyHash[wxT("(C)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/coffee.gif' align='center'>");
	SmileyHash[wxT("(c)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/coffee.gif' align='center'>");
	SmileyHash[wxT("(^)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile054.gif' align='center'>");
	SmileyHash[wxT("(pi)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/pi.gif' align='center'>");
	SmileyHash[wxT("(||)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile047.gif' align='center'>");
	SmileyHash[wxT("(M)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/m.gif' align='center'>");
	SmileyHash[wxT("(@)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/cat.gif' align='center'>");
	SmileyHash[wxT("(sn)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/sn.gif' align='center'>");
	SmileyHash[wxT("(bah)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/bah.gif' align='center'>");
	SmileyHash[wxT("(tu)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile042.gif' align='center'>");
	SmileyHash[wxT("(&)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/dog.gif' align='center'>");
	SmileyHash[wxT(":-[")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile034.gif' align='center'>");
	SmileyHash[wxT(":[")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile034.gif' align='center'>");
	SmileyHash[wxT("(?)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile038.gif' align='center'>");
	SmileyHash[wxT("({)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile026.gif' align='center'>");
	SmileyHash[wxT("(})")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile025.gif' align='center'>");
	SmileyHash[wxT("(pl)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile048.gif' align='center'>");
	SmileyHash[wxT("(I)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/light.gif' align='center'>");
	SmileyHash[wxT("(i)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/light.gif' align='center'>");
	SmileyHash[wxT("(8)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/music.gif' align='center'>");
	SmileyHash[wxT("(ip)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/ip.gif' align='center'>");
	SmileyHash[wxT("(S)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile021.gif' align='center'>");
	SmileyHash[wxT("(*)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile022.gif' align='center'>");
	SmileyHash[wxT("(R)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile024.gif' align='center'>");
	SmileyHash[wxT("(r)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile024.gif' align='center'>");
	SmileyHash[wxT("(#)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile023.gif' align='center'>");
	SmileyHash[wxT("(li)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile052.gif' align='center'>");
	SmileyHash[wxT("(st)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile051.gif' align='center'>");
	SmileyHash[wxT("(um)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/um.gif' align='center'>");
	SmileyHash[wxT("(co)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/co.gif'> align='center'");
	SmileyHash[wxT("(mp)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/mp.gif' align='center'>");
	SmileyHash[wxT("(T)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/phone.gif' align='center'>");
	SmileyHash[wxT("(t)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/phone.gif' align='center'>");
	SmileyHash[wxT("(E)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/mail.gif' align='center'>");
	SmileyHash[wxT("(e)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/mail.gif' align='center'>");
	SmileyHash[wxT("(ap)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/ap.gif' align='center'>");
	SmileyHash[wxT("(au)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/au.gif' align='center'>");
	SmileyHash[wxT("(~)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/movie.gif' align='center'>");
	SmileyHash[wxT("(O)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/time.gif' align='center'>");
	SmileyHash[wxT("(o)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/time.gif' align='center'>");
	SmileyHash[wxT("(so)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/so.gif' align='center'>");
	SmileyHash[wxT("(ci)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/ci.gif' align='center'>");
	SmileyHash[wxT("(yn)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile046.gif' align='center'>");
	SmileyHash[wxT("(h5)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile045.gif' align='center'>");
	SmileyHash[wxT("(xx)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile044.gif' align='center'>");
	SmileyHash[wxT("(mo)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/mo.gif' align='center'>"); 
	SmileyHash[wxT(":)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile001.gif' align='center'>");
	SmileyHash[wxT(":-)")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile001.gif' align='center'>");
	SmileyHash[wxT(":D")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile002.gif' align='center'>");
	SmileyHash[wxT(":-D")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile002.gif' align='center'>");
	SmileyHash[wxT(":>")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile002.gif' align='center'>");
	SmileyHash[wxT(":-d")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile002.gif' align='center'>");
	SmileyHash[wxT(":O")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile008.gif' align='center'>");
	SmileyHash[wxT(":o")] = wxT("<img src='") + ImagePath + wxT("resources/smileys/smile008.gif' align='center'>");

	HtmlHash[wxT("<")] = wxT("&#60;");
	HtmlHash[wxT(">")] = wxT("&#62;");
	HtmlHash[wxT("\n")] = wxT("<br>");
	HtmlHash[wxT("&apos;")] = wxT("'");
	HtmlHash[wxT("  ")] = wxT("&nbsp; ");
	
	// Build all Regex we'll use
	wxRegEx reHtmlStyleColorTag(wxT("^·\\$#[0-9a-fA-F]{6}"));
	wxRegEx reIndexedColorTag(wxT("^·\\$([0-9]|([1-5][0-9])|(6[0-8]))"));
	wxRegEx reRGBStyleColor(wxT("^·\\$\\((([0-1]{0,1}[0-9]{0,2})|(2{1}[0-5]{0,2})),(([0-1]{0,1}[0-9]{0,2})|(2{1}[0-5]{0,2})),(([0-1]{0,1}[0-9]{0,2})|(2{1}[0-5]{0,2}))\\)"));
	wxRegEx reUrl(wxT("^(((http|ftp|https|ftps)://)|www\\.).[^ [.'.][.\".][.{.][.}.][.[.][.].][.(.][.).][.<.][.>.]]*"));
	int ColorTagReplaced = 0;
	bool BoldTagOpened = false;
	bool ItalicTagOpened = false;
	bool StrikeTagOpened = false;
	bool UnderlineTagOpened = false;
	
	while(SourceCopy.Len() > 0)
	{
		if(reHtmlStyleColorTag.Matches(SourceCopy) == true)
		{
			if(ParseMSNPlusCodes == true)
			{
				Result += wxT("<font color='") + SourceCopy.Mid(2, 7) + wxT("'>");
				ColorTagReplaced++;
			}
			SourceCopy.Remove(0, 9);
		}
		else if(reIndexedColorTag.Matches(SourceCopy) == true)
		{
			wxString Color = reIndexedColorTag.GetMatch(SourceCopy);
			if(ParseMSNPlusCodes == true)
			{
				Result += wxT("<font color='") + IndexedColorHash[Color] + wxT("'>");
				ColorTagReplaced++;
			}
			SourceCopy.Remove(0, Color.Len());
		}
		else if(reRGBStyleColor.Matches(SourceCopy) == true)
		{
			wxString Color = reRGBStyleColor.GetMatch(SourceCopy);
			if(ParseMSNPlusCodes == true)
			{
				wxLogMessage(Color);
				wxStringTokenizer Token(Color.Mid(3, Color.Len()-4), wxT(","));
				long R, G, B;
				wxString StrR, StrG, StrB;
				StrR = Token.GetNextToken();
				StrG = Token.GetNextToken();
				StrB = Token.GetNextToken();
				StrR.ToLong(&R);
				StrG.ToLong(&G);
				StrB.ToLong(&B);
				Result += wxString::Format(wxT("<font color='#%.2x%.2x%.2x'>"), static_cast<int>(R), static_cast<int>(G), static_cast<int>(B));
				ColorTagReplaced++;
			}
			SourceCopy.Remove(0, Color.Len());
		}
		else if(reUrl.Matches(SourceCopy) == true && ParseURL == true)
		{
			wxString Link = reUrl.GetMatch(SourceCopy);
			wxString CleanedLink = Link;
			Result += wxT("<a href='") + CleanedLink + wxT("'>") + CleanedLink + wxT("</a>");
			SourceCopy.Remove(0, Link.Len());
		}
		else if(SourceCopy.StartsWith(wxT("·0")))
		{
			// Close all opened <font>
			while(ColorTagReplaced > 0)
			{
				Result += wxT("</font>");
				ColorTagReplaced--;
			}
			SourceCopy.Remove(0, 2);
		}
		else if(SourceCopy.StartsWith(wxT("·#")))
		{
			if(ParseMSNPlusCodes == true)
			{
				Result += (BoldTagOpened == true) ? wxT("</b>") : wxT("<b>");
				BoldTagOpened = !BoldTagOpened;
			}
			SourceCopy.Remove(0, 2);
		}
		else if(SourceCopy.StartsWith(wxT("·@")))
		{
			if(ParseMSNPlusCodes == true)
			{
				Result += (UnderlineTagOpened == true) ? wxT("</u>") : wxT("<u>");
				UnderlineTagOpened = !UnderlineTagOpened;
			}
			SourceCopy.Remove(0, 2);
		}
		else if(SourceCopy.StartsWith(wxT("·&")))
		{
			if(ParseMSNPlusCodes == true)
			{
				Result += (ItalicTagOpened == true) ? wxT("</i>") : wxT("<i>");
				ItalicTagOpened = !ItalicTagOpened;
			}
			SourceCopy.Remove(0, 2);
		}
		else if(SourceCopy.StartsWith(wxT("·'")))
		{
			if(ParseMSNPlusCodes == true)
			{
				Result += (StrikeTagOpened == true) ? wxT("</strike>") : wxT("<strike>");
				StrikeTagOpened = !StrikeTagOpened;
			}
			SourceCopy.Remove(0, 2);
		}
		else {
			if(ParseEmoticons == true)
			{
				StringHash::iterator it;
				for(it = SmileyHash.begin(); it != SmileyHash.end(); ++it)
				{
					if(SourceCopy.StartsWith(it->first) == true)
					{
						Result += it->second;
						SourceCopy.Remove(0, (it->first).Len());
						break;
					}
				}
			}
			
			if(CleanHtml == true)
			{
				bool Found = false;
				StringHash::iterator it;
				for(it = HtmlHash.begin(); it != HtmlHash.end(); ++it)
				{
					if(SourceCopy.StartsWith(it->first) == true)
					{
						Result += it->second;
						SourceCopy.Remove(0, (it->first).Len());
						Found = true;
						break;
					}
				}
				
				// Workaround to a wxHtmlBug
				if(Found == false)
				{
					if(SourceCopy[0] == wxChar(' ') && Result.Matches(wxT("*<img*>")) == true)
					{
						Result += wxT("&nbsp;");
					}
					else
					{
						Result += SourceCopy[0];
					}
					SourceCopy.Remove(0, 1);
				}
			}
			else
			{
				Result += SourceCopy[0];
				SourceCopy.Remove(0, 1);
			}
		}
	}

	if(ParseMSNPlusCodes == true)
	{
		// Close all <font> tag left opened	
		while(ColorTagReplaced > 0)
		{
			Result += wxT("</font>");
			ColorTagReplaced--;
		}
		
		// Same thing for bold, underline etc etc
		if(BoldTagOpened == true)
		{
			Result += wxT("</b>");
		}
		
		if(UnderlineTagOpened == true)
		{
			Result += wxT("</u>");
		}
		
		if(StrikeTagOpened == true)
		{
			Result += wxT("</strike>");
		}
		
		if(ItalicTagOpened == true)
		{
			Result += wxT("</i>");
		}
		
		// Removes redundant "·0"
		Result.Replace(wxT("·0"), wxEmptyString);
	}
	
	return wxT("<a href='pipo'>Je teste un truc</a>");//Result;
}

//Url Encoding (code from uri.cpp)

bool IsAlpha(const wxChar& c)
{   
    return (c >= wxT('a') && c <= wxT('z')) || (c >= wxT('A') && c <= wxT('Z'));  
}

bool IsDigit(const wxChar& c)
{  
    return c >= wxT('0') && c <= wxT('9');        
}

bool IsHex(const wxChar& c)
{   
    return IsDigit(c) || (c >= wxT('a') && c <= wxT('f')) || (c >= wxT('A') && c <= wxT('F')); 
}

bool IsUnreserved (const wxChar& c)
{   
    return IsAlpha(c) || IsDigit(c) ||
           c == wxT('-') ||
           c == wxT('.') ||
           c == wxT('_') ||
           c == wxT('~') //tilde
           ;
}

void Escape(wxString& s, const wxChar& c)
{
    const wxChar* hdig = wxT("0123456789abcdef");
    s += wxT('%');
    s += hdig[(c >> 4) & 15];
    s += hdig[c & 15];
}

bool IsEscape(wxChar * uri)
{
    // pct-encoded   = "%" HEXDIG HEXDIG
    if(*uri == wxT('%') && IsHex(*(uri+1)) && IsHex(*(uri+2)))
        return true;
    else
        return false;
}

wxString SPACE::UrlEncode(const wxString & Str)
{
    /*const wxChar * ptr = Str;
    wxChar *uri = (wxChar *) ptr;*/
    wxChar *uri = (wxChar *) Str.c_str();
    wxString final;
    while(*uri)
    {
        if(IsUnreserved(*uri))
            final += *uri++;
        else if (IsEscape(uri))
        {
            final += *uri++;
            final += *uri++;
            final += *uri++;
        }
        else
            Escape(final, *uri++);
    }  
    return final;
}

wxChar CharToHex(const wxChar& c)
{
    if ((c >= wxT('A')) && (c <= wxT('Z'))) return wxChar(c - wxT('A') + 0x0A);
    if ((c >= wxT('a')) && (c <= wxT('z'))) return wxChar(c - wxT('a') + 0x0a);
    if ((c >= wxT('0')) && (c <= wxT('9'))) return wxChar(c - wxT('0') + 0x00);

    return 0;
}

wxChar TranslateEscape(const wxChar* s)
{
    return /*wx_truncate_cast(*/(wxChar)((CharToHex(s[0]) << 4 ) | CharToHex(s[1]));
}

wxString SPACE::UrlDecode(const wxString & uri)
{
    wxString new_uri;
    
    for(size_t i = 0; i < uri.length(); ++i)
    {
        if (uri[i] == wxT('%'))
        {
            new_uri += TranslateEscape( &(uri.c_str()[i+1]) );
            i += 2;
        }
        else
            new_uri += uri[i];
    }

    return new_uri;
}

void SPACE::WizzFrame(wxFrame * frame, int nb, int decal)
{
    wxPoint pt = frame->GetPosition();
    bool a  = false;
    for(int i = 0 ; i < nb ; ++i)
    {
        if(a)
        {
            frame->Move(pt.x+decal, pt.y+decal);
            a = false;
        }
        else
        {
            frame->Move(pt.x-decal, pt.y-decal);
            a = true;
        }
    }
    frame->Move(pt);
}

// WindowLog
#define CLASS CLogWindow

SPACE::CLASS::CLASS(wxWindow * parent, wxString title)
    : wxLogWindow(parent, title, true, true)
{}

bool SPACE::CLASS::OnFrameClose(wxFrame *frame)
{
    return false;
}

#undef CLASS

#define CLASS CPrefs

wxString SPACE::CLASS::m_path = wxString();

void SPACE::CLASS::Init()
{
	wxString path(wxPathOnly(wxTheApp->argv[0]) + wxT("/"));
	m_path = path;
}

wxString SPACE::CLASS::GetPath()
{
	return m_path;
}

wxString SPACE::CLASS::LocateRSC(const wxString & str)
{
	return m_path + wxString(_T("resources/")) + str;
}

#undef CLASS
