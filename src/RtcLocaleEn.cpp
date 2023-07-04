/*-------------------------------------------------------------------------
RTC library

Written by Michael C. Miller.

I invest time and resources providing this open source code,
please support me by dontating (see https://github.com/Makuna/Rtc)

-------------------------------------------------------------------------
This file is part of the Makuna/Rtc library.

Rtc is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

Rtc is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with Rtc.  If not, see
<http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------*/

#include <Arduino.h>
#include "RtcUtility.h"
#include "RtcTimeZone.h"
#include "RtcLocaleEn.h"

// due to how PROGMEM works, strings must be declared standalone
//
const char stringACDT[] PROGMEM = "ACDT";
const char stringACST[] PROGMEM = "ACST";
const char stringACT[] PROGMEM = "ACT";
const char stringACWST[] PROGMEM = "ACWST";
const char stringADT[] PROGMEM = "ADT";
const char stringAEDT[] PROGMEM = "AEDT";
const char stringAEST[] PROGMEM = "AEST";
const char stringAET[] PROGMEM = "AET";
const char stringAFT[] PROGMEM = "AFT";
const char stringAKDT[] PROGMEM = "AKDT";
const char stringAKST[] PROGMEM = "AKST";
const char stringALMT[] PROGMEM = "ALMT";
const char stringAMST[] PROGMEM = "AMST";
const char stringAMT[] PROGMEM = "AMT";
const char stringANAT[] PROGMEM = "ANAT";
const char stringAQTT[] PROGMEM = "AQTT";
const char stringART[] PROGMEM = "ART";
const char stringAST[] PROGMEM = "AST";
const char stringAWST[] PROGMEM = "AWST";
const char stringAZOST[] PROGMEM = "AZOST";
const char stringAZOT[] PROGMEM = "AZOT";
const char stringAZT[] PROGMEM = "AZT";
const char stringBNT[] PROGMEM = "BNT";
const char stringBIOT[] PROGMEM = "BIOT";
const char stringBIT[] PROGMEM = "BIT";
const char stringBOT[] PROGMEM = "BOT";
const char stringBRST[] PROGMEM = "BRST";
const char stringBRT[] PROGMEM = "BRT";
const char stringBST[] PROGMEM = "BST";
const char stringBTT[] PROGMEM = "BTT";
const char stringCAT[] PROGMEM = "CAT";
const char stringCCT[] PROGMEM = "CCT";
const char stringCDT[] PROGMEM = "CDT";
const char stringCEST[] PROGMEM = "CEST";
const char stringCET[] PROGMEM = "CET";
const char stringCHADT[] PROGMEM = "CHADT";
const char stringCHAST[] PROGMEM = "CHAST";
const char stringCHOT[] PROGMEM = "CHOT";
const char stringCHOST[] PROGMEM = "CHOST";
const char stringCHST[] PROGMEM = "CHST";
const char stringCHUT[] PROGMEM = "CHUT";
const char stringCIST[] PROGMEM = "CIST";
const char stringCKT[] PROGMEM = "CKT";
const char stringCLST[] PROGMEM = "CLST";
const char stringCLT[] PROGMEM = "CLT";
const char stringCOST[] PROGMEM = "COST";
const char stringCOT[] PROGMEM = "COT";
const char stringCST[] PROGMEM = "CST";
const char stringCT[] PROGMEM = "CT";
const char stringCVT[] PROGMEM = "CVT";
const char stringCWST[] PROGMEM = "CWST";
const char stringCXT[] PROGMEM = "CXT";
const char stringDAVT[] PROGMEM = "DAVT";
const char stringDDUT[] PROGMEM = "DDUT";
const char stringDFT[] PROGMEM = "DFT";
const char stringEASST[] PROGMEM = "EASST";
const char stringEAST[] PROGMEM = "EAST";
const char stringEAT[] PROGMEM = "EAT";
const char stringECT[] PROGMEM = "ECT";
const char stringEDT[] PROGMEM = "EDT";
const char stringEEST[] PROGMEM = "EEST";
const char stringEET[] PROGMEM = "EET";
const char stringEGST[] PROGMEM = "EGST";
const char stringEGT[] PROGMEM = "EGT";
const char stringEST[] PROGMEM = "EST";
const char stringET[] PROGMEM = "ET";
const char stringFET[] PROGMEM = "FET";
const char stringFJT[] PROGMEM = "FJT";
const char stringFKST[] PROGMEM = "FKST";
const char stringFKT[] PROGMEM = "FKT";
const char stringFNT[] PROGMEM = "FNT";
const char stringGALT[] PROGMEM = "GALT";
const char stringGAMT[] PROGMEM = "GAMT";
const char stringGET[] PROGMEM = "GET";
const char stringGFT[] PROGMEM = "GFT";
const char stringGILT[] PROGMEM = "GILT";
const char stringGIT[] PROGMEM = "GIT";
const char stringGMT[] PROGMEM = "GMT";
const char stringGST[] PROGMEM = "GST";
const char stringGYT[] PROGMEM = "GYT";
const char stringHDT[] PROGMEM = "HDT";
const char stringHAEC[] PROGMEM = "HAEC";
const char stringHST[] PROGMEM = "HST";
const char stringHKT[] PROGMEM = "HKT";
const char stringHMT[] PROGMEM = "HMT";
const char stringHOVST[] PROGMEM = "HOVST";
const char stringHOVT[] PROGMEM = "HOVT";
const char stringICT[] PROGMEM = "ICT";
const char stringIDLW[] PROGMEM = "IDLW";
const char stringIDT[] PROGMEM = "IDT";
const char stringIOT[] PROGMEM = "IOT";
const char stringIRDT[] PROGMEM = "IRDT";
const char stringIRKT[] PROGMEM = "IRKT";
const char stringIRST[] PROGMEM = "IRST";
const char stringIST[] PROGMEM = "IST";
const char stringJST[] PROGMEM = "JST";
const char stringKALT[] PROGMEM = "KALT";
const char stringKGT[] PROGMEM = "KGT";
const char stringKOST[] PROGMEM = "KOST";
const char stringKRAT[] PROGMEM = "KRAT";
const char stringKST[] PROGMEM = "KST";
const char stringLHST[] PROGMEM = "LHST";
const char stringLINT[] PROGMEM = "LINT";
const char stringMAGT[] PROGMEM = "MAGT";
const char stringMART[] PROGMEM = "MART";
const char stringMAWT[] PROGMEM = "MAWT";
const char stringMDT[] PROGMEM = "MDT";
const char stringMET[] PROGMEM = "MET";
const char stringMEST[] PROGMEM = "MEST";
const char stringMHT[] PROGMEM = "MHT";
const char stringMIST[] PROGMEM = "MIST";
const char stringMIT[] PROGMEM = "MIT";
const char stringMMT[] PROGMEM = "MMT";
const char stringMSK[] PROGMEM = "MSK";
const char stringMST[] PROGMEM = "MST";
const char stringMUT[] PROGMEM = "MUT";
const char stringMVT[] PROGMEM = "MVT";
const char stringMYT[] PROGMEM = "MYT";
const char stringNCT[] PROGMEM = "NCT";
const char stringNDT[] PROGMEM = "NDT";
const char stringNFT[] PROGMEM = "NFT";
const char stringNOVT[] PROGMEM = "NOVT";
const char stringNPT[] PROGMEM = "NPT";
const char stringNST[] PROGMEM = "NST";
const char stringNT[] PROGMEM = "NT";
const char stringNUT[] PROGMEM = "NUT";
const char stringNZDT[] PROGMEM = "NZDT";
const char stringNZST[] PROGMEM = "NZST";
const char stringOMST[] PROGMEM = "OMST";
const char stringORAT[] PROGMEM = "ORAT";
const char stringPDT[] PROGMEM = "PDT";
const char stringPET[] PROGMEM = "PET";
const char stringPETT[] PROGMEM = "PETT";
const char stringPGT[] PROGMEM = "PGT";
const char stringPHOT[] PROGMEM = "PHOT";
const char stringPHT[] PROGMEM = "PHT";
const char stringPHST[] PROGMEM = "PHST";
const char stringPKT[] PROGMEM = "PKT";
const char stringPMDT[] PROGMEM = "PMDT";
const char stringPMST[] PROGMEM = "PMST";
const char stringPONT[] PROGMEM = "PONT";
const char stringPST[] PROGMEM = "PST";
const char stringPWT[] PROGMEM = "PWT";
const char stringPYST[] PROGMEM = "PYST";
const char stringPYT[] PROGMEM = "PYT";
const char stringRET[] PROGMEM = "RET";
const char stringROTT[] PROGMEM = "ROTT";
const char stringSAKT[] PROGMEM = "SAKT";
const char stringSAMT[] PROGMEM = "SAMT";
const char stringSAST[] PROGMEM = "SAST";
const char stringSBT[] PROGMEM = "SBT";
const char stringSCT[] PROGMEM = "SCT";
const char stringSDT[] PROGMEM = "SDT";
const char stringSGT[] PROGMEM = "SGT";
const char stringSLST[] PROGMEM = "SLST";
const char stringSRET[] PROGMEM = "SRET";
const char stringSRT[] PROGMEM = "SRT";
const char stringSST[] PROGMEM = "SST";
const char stringSYOT[] PROGMEM = "SYOT";
const char stringTAHT[] PROGMEM = "TAHT";
const char stringTHA[] PROGMEM = "THA";
const char stringTFT[] PROGMEM = "TFT";
const char stringTJT[] PROGMEM = "TJT";
const char stringTKT[] PROGMEM = "TKT";
const char stringTLT[] PROGMEM = "TLT";
const char stringTMT[] PROGMEM = "TMT";
const char stringTRT[] PROGMEM = "TRT";
const char stringTOT[] PROGMEM = "TOT";
const char stringTVT[] PROGMEM = "TVT";
const char stringULAST[] PROGMEM = "ULAST";
const char stringULAT[] PROGMEM = "ULAT";
const char stringUTC[] PROGMEM = "UTC";
const char stringUYST[] PROGMEM = "UYST";
const char stringUYT[] PROGMEM = "UYT";
const char stringUZT[] PROGMEM = "UZT";
const char stringVET[] PROGMEM = "VET";
const char stringVLAT[] PROGMEM = "VLAT";
const char stringVOLT[] PROGMEM = "VOLT";
const char stringVOST[] PROGMEM = "VOST";
const char stringVUT[] PROGMEM = "VUT";
const char stringWAKT[] PROGMEM = "WAKT";
const char stringWAST[] PROGMEM = "WAST";
const char stringWAT[] PROGMEM = "WAT";
const char stringWEST[] PROGMEM = "WEST";
const char stringWET[] PROGMEM = "WET";
const char stringWIB[] PROGMEM = "WIB";
const char stringWIT[] PROGMEM = "WIT";
const char stringWITA[] PROGMEM = "WITA";
const char stringWGST[] PROGMEM = "WGST";
const char stringWGT[] PROGMEM = "WGT";
const char stringWST[] PROGMEM = "WST";
const char stringYAKT[] PROGMEM = "YAKT";
const char stringYEKT[] PROGMEM = "YEKT";


// collection in alphabetic order of abbreviation
// duplicates commented out randomly (why are there duplicates in a standard?!)
// IF YOU MODIFY THIS to your locale, please goto github and add an issue
// 
const RtcTimeZone c_tzEn[] PROGMEM =
{
	{ stringACDT, 10 * 60 + 30 },    // Australian Central Daylight Saving Time
	{ stringACST, 9 * 60 + 30 },    // Australian Central Standard Time
	{ stringACT, -5 * 60 },    // Acre Time
//	{ stringACT, 8 * 60 + 0 },    // ASEAN Common Time (proposed)
	{ stringACWST, 8 * 60 + 45 },    // Australian Central Western Standard Time (unofficial)
	{ stringADT, -3 * 60 },    // Atlantic Daylight Time
	{ stringAEDT, 11 * 60 },    // Australian Eastern Daylight Saving Time
	{ stringAEST, 10 * 60 },    // Australian Eastern Standard Time
	{ stringAET, 10 * 60 },    // Australian Eastern Time
	{ stringAFT, 4 * 60 + 30 },    // Afghanistan Time
	{ stringAKDT, -8 * 60 },    // Alaska Daylight Time
	{ stringAKST, -9 * 60 },    // Alaska Standard Time
	{ stringALMT, 6 * 60 },    // Alma-Ata Time[1]
	{ stringAMST, -3 * 60 },    // Amazon Summer Time (Brazil)[2]
	{ stringAMT, -4 * 60 },    // Amazon Time (Brazil)[3]
//	{ stringAMT, 4 * 60 },    // Armenia Time
	{ stringANAT, 12 * 60 },    // Anadyr Time[4]
	{ stringAQTT, 5 * 60 },    // Aqtobe Time[5]
	{ stringART, -3 * 60 },    // Argentina Time
//	{ stringAST, 3 * 60 },    // Arabia Standard Time
	{ stringAST, -4 * 60 },    // Atlantic Standard Time
	{ stringAWST, 8 * 60 },    // Australian Western Standard Time
	{ stringAZOST, 0 },    // Azores Summer Time
	{ stringAZOT, -1 * 60 },    // Azores Standard Time
	{ stringAZT, 4 * 60 },    // Azerbaijan Time
	{ stringBNT, 8 * 60 },    // Brunei Time
	{ stringBIOT, 6 * 60 },    // British Indian Ocean Time
	{ stringBIT, -12 * 60 },    // Baker Island Time
	{ stringBOT, -4 * 60 },    // Bolivia Time
	{ stringBRST, -2 * 60 },    // Brasília Summer Time
	{ stringBRT, -3 * 60 },    // Brasília Time
//	{ stringBST, 6 * 60 },    // Bangladesh Standard Time
//	{ stringBST, 11 * 60 },    // Bougainville Standard Time[6]
	{ stringBST, 1 * 60 },    // British Summer Time (British Standard Time from Mar 1968 to Oct 1971)
	{ stringBTT, 6 * 60 },    // Bhutan Time
	{ stringCAT, 2 * 60 },    // Central Africa Time
	{ stringCCT, 6 * 60 + 30 },    // Cocos Islands Time
	{ stringCDT, -5 * 60 },    // Central Daylight Time (North America)
//	{ stringCDT, -4 * 60 },    // Cuba Daylight Time[7]
	{ stringCEST, 2 * 60 },    // Central European Summer Time
	{ stringCET, 1 * 60 },    // Central European Time
	{ stringCHADT, 13 * 60 + 45 },    // Chatham Daylight Time
	{ stringCHAST, 12 * 60 + 45 },    // Chatham Standard Time
	{ stringCHOT, 8 * 60 },    // Choibalsan Standard Time
	{ stringCHOST, 9 * 60 },    // Choibalsan Summer Time
	{ stringCHST, 10 * 60 },    // Chamorro Standard Time
	{ stringCHUT, 10 * 60 },    // Chuuk Time
	{ stringCIST, -8 * 60 },    // Clipperton Island Standard Time
	{ stringCKT, -10 * 60 },    // Cook Island Time
	{ stringCLST, -3 * 60 },    // Chile Summer Time
	{ stringCLT, -4 * 60 },    // Chile Standard Time
	{ stringCOST, -4 * 60 },    // Colombia Summer Time
	{ stringCOT, -5 * 60 },    // Colombia Time
	{ stringCST, -6 * 60 },    // Central Standard Time (North America)
//	{ stringCST, 8 * 60 },    // China Standard Time
//	{ stringCST, -5 * 60 },    // Cuba Standard Time
	{ stringCT, -6 * 60 },    // Central Time
	{ stringCVT, -1 * 60 },    // Cape Verde Time
	{ stringCWST, 8 * 60 + 45 },    // Central Western Standard Time (Australia) unofficial
	{ stringCXT, 7 * 60 },    // Christmas Island Time
	{ stringDAVT, 7 * 60 },    // Davis Time
	{ stringDDUT, 10 * 60 },    // Dumont d'Urville Time
	{ stringDFT, 1 * 60 },    // AIX-specific equivalent of Central European Time[NB 1]
	{ stringEASST, -5 * 60 },    // Easter Island Summer Time
	{ stringEAST, -6 * 60 },    // Easter Island Standard Time
	{ stringEAT, 3 * 60 },    // East Africa Time
	{ stringECT, -4 * 60 },    // Eastern Caribbean Time (does not recognise DST)
//	{ stringECT, -5 * 60 },    // Ecuador Time
	{ stringEDT, -4 * 60 },    // Eastern Daylight Time (North America)
	{ stringEEST, 3 * 60 },    // Eastern European Summer Time
	{ stringEET, 2 * 60 },    // Eastern European Time
	{ stringEGST, 0 },    // Eastern Greenland Summer Time
	{ stringEGT, -1 * 60 },    // Eastern Greenland Time
	{ stringEST, -5 * 60 },    // Eastern Standard Time (North America)
	{ stringET, -5 * 60 },    // Eastern Time (North America)
	{ stringFET, 3 * 60 },    // Further-eastern European Time
	{ stringFJT, 12 * 60 },    // Fiji Time
	{ stringFKST, -3 * 60 },    // Falkland Islands Summer Time
	{ stringFKT, -4 * 60 },    // Falkland Islands Time
	{ stringFNT, -2 * 60 },    // Fernando de Noronha Time
	{ stringGALT, -6 * 60 },    // Galápagos Time
	{ stringGAMT, -9 * 60 },    // Gambier Islands Time
	{ stringGET, 4 * 60 },    // Georgia Standard Time
	{ stringGFT, -3 * 60 },    // French Guiana Time
	{ stringGILT, 12 * 60 },    // Gilbert Island Time
	{ stringGIT, -9 * 60 },    // Gambier Island Time
	{ stringGMT, 0 },    // Greenwich Mean Time
//	{ stringGST, -2 * 60 },    // South Georgia and the South Sandwich Islands Time
	{ stringGST, 4 * 60 },    // Gulf Standard Time
	{ stringGYT, -4 * 60 },    // Guyana Time
	{ stringHDT, -9 * 60 },    // Hawaii–Aleutian Daylight Time
	{ stringHAEC, 2 * 60 },    // Heure Avancée d'Europe Centrale French-language name for CEST
	{ stringHST, -10 * 60 },    // Hawaii–Aleutian Standard Time
	{ stringHKT, 8 * 60 },    // Hong Kong Time
	{ stringHMT, 5 * 60 },    // Heard and McDonald Islands Time
	{ stringHOVST, 8 * 60 },    // Hovd Summer Time (not used from 2017-present)
	{ stringHOVT, 7 * 60 },    // Hovd Time
	{ stringICT, 7 * 60 },    // Indochina Time
	{ stringIDLW, -12 * 60 },    // International Date Line West time zone
	{ stringIDT, 3 * 60 },    // Israel Daylight Time
	{ stringIOT, 3 * 60 },    // Indian Ocean Time
	{ stringIRDT, 4 * 60 + 30 },    // Iran Daylight Time
	{ stringIRKT, 8 * 60 },    // Irkutsk Time
	{ stringIRST, 3 * 60 + 30 },    // Iran Standard Time
//	{ stringIST, 5 * 60 + 30 },    // Indian Standard Time
//	{ stringIST, 1 * 60 },    // Irish Standard Time[8]
	{ stringIST, 2 * 60 },    // Israel Standard Time
	{ stringJST, 9 * 60 },    // Japan Standard Time
	{ stringKALT, 2 * 60 },    // Kaliningrad Time
	{ stringKGT, 6 * 60 },    // Kyrgyzstan Time
	{ stringKOST, 11 * 60 },    // Kosrae Time
	{ stringKRAT, 7 * 60 },    // Krasnoyarsk Time
	{ stringKST, 9 * 60 },    // Korea Standard Time
	{ stringLHST, 10 * 60 + 30 },    // Lord Howe Standard Time
	{ stringLHST, 11 * 60 },    // Lord Howe Summer Time
	{ stringLINT, 14 * 60 },    // Line Islands Time
	{ stringMAGT, 12 * 60 },    // Magadan Time
	{ stringMART, -(9 * 60 + 30) },    // Marquesas Islands Time
	{ stringMAWT, 5 * 60 },    // Mawson Station Time
	{ stringMDT, -6 * 60 },    // Mountain Daylight Time (North America)
	{ stringMET, 1 * 60 },    // Middle European Time (same zone as CET)
	{ stringMEST, 2 * 60 },    // Middle European Summer Time (same zone as CEST)
	{ stringMHT, 12 * 60 },    // Marshall Islands Time
	{ stringMIST, 11 * 60 },    // Macquarie Island Station Time
	{ stringMIT, -(9 * 60 + 30) },    // Marquesas Islands Time
	{ stringMMT, 6 * 60 + 30 },    // Myanmar Standard Time
	{ stringMSK, 3 * 60 },    // Moscow Time
	{ stringMST, 8 * 60 },    // Malaysia Standard Time
	{ stringMST, -7 * 60 },    // Mountain Standard Time (North America)
	{ stringMUT, 4 * 60 },    // Mauritius Time
	{ stringMVT, 5 * 60 },    // Maldives Time
	{ stringMYT, 8 * 60 },    // Malaysia Time
	{ stringNCT, 11 * 60 },    // New Caledonia Time
	{ stringNDT, -(2 * 60 + 30) },    // Newfoundland Daylight Time
	{ stringNFT, 11 * 60 },    // Norfolk Island Time
	{ stringNOVT, 7 * 60 },    // Novosibirsk Time [9]
	{ stringNPT, 5 * 60 + 45 },    // Nepal Time
	{ stringNST, -(3 * 60 + 30) },    // Newfoundland Standard Time
	{ stringNT, -(3 * 60 + 30) },    // Newfoundland Time
	{ stringNUT, -11 * 60 },    // Niue Time
	{ stringNZDT, 13 * 60 },    // New Zealand Daylight Time
	{ stringNZST, 12 * 60 },    // New Zealand Standard Time
	{ stringOMST, 6 * 60 },    // Omsk Time
	{ stringORAT, 5 * 60 },    // Oral Time
	{ stringPDT, -7 * 60 },    // Pacific Daylight Time (North America)
	{ stringPET, -5 * 60 },    // Peru Time
	{ stringPETT, 12 * 60 },    // Kamchatka Time
	{ stringPGT, 10 * 60 },    // Papua New Guinea Time
	{ stringPHOT, 13 * 60 },    // Phoenix Island Time
	{ stringPHT, 8 * 60 },    // Philippine Time
	{ stringPHST, 8 * 60 },    // Philippine Standard Time
	{ stringPKT, 5 * 60 },    // Pakistan Standard Time
	{ stringPMDT, -2 * 60 },    // Saint Pierre and Miquelon Daylight Time
	{ stringPMST, -3 * 60 },    // Saint Pierre and Miquelon Standard Time
	{ stringPONT, 11 * 60 },    // Pohnpei Standard Time
	{ stringPST, -8 * 60 },    // Pacific Standard Time (North America)
	{ stringPWT, 9 * 60 },    // Palau Time[10]
	{ stringPYST, -3 * 60 },    // Paraguay Summer Time[11]
	{ stringPYT, -4 * 60 },    // Paraguay Time[12]
	{ stringRET, 4 * 60 },    // Réunion Time
	{ stringROTT, -3 * 60 },    // Rothera Research Station Time
	{ stringSAKT, 11 * 60 },    // Sakhalin Island Time
	{ stringSAMT, 4 * 60 },    // Samara Time
	{ stringSAST, 2 * 60 },    // South African Standard Time
	{ stringSBT, 11 * 60 },    // Solomon Islands Time
	{ stringSCT, 4 * 60 },    // Seychelles Time
	{ stringSDT, -10 * 60 },    // Samoa Daylight Time
	{ stringSGT, 8 * 60 },    // Singapore Time
	{ stringSLST, 5 * 60 + 30 },    // Sri Lanka Standard Time
	{ stringSRET, 11 * 60 },    // Srednekolymsk Time
	{ stringSRT, -3 * 60 },    // Suriname Time
//	{ stringSST, -11 * 60 },    // Samoa Standard Time
	{ stringSST, 8 * 60 },    // Singapore Standard Time
	{ stringSYOT, 3 * 60 },    // Showa Station Time
	{ stringTAHT, -10 * 60 },    // Tahiti Time
	{ stringTHA, 7 * 60 },    // Thailand Standard Time
	{ stringTFT, 5 * 60 },    // French Southern and Antarctic Time[13]
	{ stringTJT, 5 * 60 },    // Tajikistan Time
	{ stringTKT, 13 * 60 },    // Tokelau Time
	{ stringTLT, 9 * 60 },    // Timor Leste Time
	{ stringTMT, 5 * 60 },    // Turkmenistan Time
	{ stringTRT, 3 * 60 },    // Turkey Time
	{ stringTOT, 13 * 60 },    // Tonga Time
	{ stringTVT, 12 * 60 },    // Tuvalu Time
	{ stringULAST, 9 * 60 },    // Ulaanbaatar Summer Time
	{ stringULAT, 8 * 60 },    // Ulaanbaatar Standard Time
	{ stringUTC, 0 },    // Coordinated Universal Time
	{ stringUYST, -2 * 60 },    // Uruguay Summer Time
	{ stringUYT, -3 * 60 },    // Uruguay Standard Time
	{ stringUZT, 5 * 60 },    // Uzbekistan Time
	{ stringVET, -4 * 60 },    // Venezuelan Standard Time
	{ stringVLAT, 10 * 60 },    // Vladivostok Time
	{ stringVOLT, 3 * 60 },    // Volgograd Time
	{ stringVOST, 6 * 60 },    // Vostok Station Time
	{ stringVUT, 11 * 60 },    // Vanuatu Time
	{ stringWAKT, 12 * 60 },    // Wake Island Time
	{ stringWAST, 2 * 60 },    // West Africa Summer Time
	{ stringWAT, 1 * 60 },    // West Africa Time
	{ stringWEST, 1 * 60 },    // Western European Summer Time
	{ stringWET,  0},    // Western European Time
	{ stringWIB, 7 * 60 },    // Western Indonesian Time
	{ stringWIT, 9 * 60 },    // Eastern Indonesian Time
	{ stringWITA, 8 * 60 },    // Central Indonesia Time
	{ stringWGST, -2 * 60 },    // West Greenland Summer Time[14]
	{ stringWGT, -3 * 60 },    // West Greenland Time[15]
	{ stringWST, 8 * 60 },    // Western Standard Time
	{ stringYAKT, 9 * 60 },    // Yakutsk Time
	{ stringYEKT, 5 * 60 }    // Yekaterinburg Time
};


uint8_t RtcLocaleEn::CharsToMonth(const char* monthChars, size_t count)
{
	uint8_t month = 0;

	// works for both standard abbreviations and full month names
	//
	if (count >= 3)
	{
		switch (tolower(monthChars[0]))
		{
		case 'j':
			if (tolower(monthChars[1]) == 'a')
				month = 1;
			else if (tolower(monthChars[2]) == 'n')
				month = 6;
			else
				month = 7;
			break;
		case 'f':
			month = 2;
			break;
		case 'a':
			month = tolower(monthChars[1]) == 'p' ? 4 : 8;
			break;
		case 'm':
			month = tolower(monthChars[2]) == 'r' ? 3 : 5;
			break;
		case 's':
			month = 9;
			break;
		case 'o':
			month = 10;
			break;
		case 'n':
			month = 11;
			break;
		case 'd':
			month = 12;
			break;
		}
	}
	return month;
}

size_t RtcLocaleEn::TimeZoneMinutesFromAbbreviation(int32_t* minutes, 
	const char* abbr)
{
	RtcTimeZone entry;

	*minutes = 0;

	size_t result = RtcTimeZone::BinarySearchProgmemTable(&entry,
		abbr,
		c_tzEn,
		countof(c_tzEn));

	if (result)
	{
		// found
		*minutes = entry.offset;
	}

	return result;
}