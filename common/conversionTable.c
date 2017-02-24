/*
 * conversionTable.c
 * 
 * Copyright 2017 Nick Schrader <nick.schrader@mailbox.org>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
 
#include <stdint.h>

#include "conversionTable.h"
#include "passwordSeed.h"

/* See documentation:
 * http://www.usb.org/developers/hidpage/Hut1_12v2.pdf
 * 
 * Conversion table for letters:
 * EN	A	M	Q	W	Y	Z
 * FR	Q	;	A	Z	Y	W	
 * DE	A	M	Q	W	Z	Y
 * 
 * Conversion table for signs:
 * Sign			$	%	&	*	(	)	-	_	=	+	;	:	'	"	,	.	/	?
 * EN									-		=		;		'		,	.	/
 * EN Shift		4	5	7	8	9	0		-		=		;		'				/
 * FR			]		1	\	5	-	6	8	=		,	.	4	3	M	
 * FR Shift			'								=						,	.	M
 * DE									/						\		,	.	
 * DE Shift		4	5	6	[	8	9		/	0	]	,	.		2			7	-
 */

const __flash uint8_t signConversionTableEN[] = {
	CS(0x21), CS(0x22), CS(0x24), CS(0x25), CS(0x26), CS(0x27), 
	LS(0x2d), CS(0x2d), LS(0x2e), CS(0x2e), LS(0x33), CS(0x33),
	LS(0x34), CS(0x34), LS(0x36), LS(0x37), LS(0x38), CS(0x38)
};

const __flash uint8_t signConversionTableFR[] = {
	LS(0x30), CS(0x34), LS(0x1e), LS(0x31), LS(0x22), LS(0x2d), 
	LS(0x23), LS(0x25), LS(0x2e), CS(0x2e), LS(0x36), LS(0x37),
	LS(0x21), LS(0x20), LS(0x10), CS(0x36), CS(0x37), CS(0x10)
};

const __flash uint8_t signConversionTableDE[] = {
	CS(0x21), CS(0x22), CS(0x23), CS(0x2f), CS(0x25), CS(0x26), 
	LS(0x38), CS(0x38), CS(0x27), CS(0x30), CS(0x36), CS(0x37),
	LS(0x31), CS(0x1f), LS(0x36), LS(0x37), CS(0x24), CS(0x2d)
};

const char signConversionTableASCII[] = {
	'$', '%', '&', '*', '(', ')', '-', '_', '=', 
	'+', ';', ':', '\'', '"', ',', '.', '/',  '?'
};

const __flash uint8_t letterConversionTableEN[] = {
	0x04, 0x10, 0x14, 0x1a, 0x1c, 0x1d
};

const __flash uint8_t letterConversionTableFR[] = {
	0x14, 0x33, 0x04, 0x1d, 0x1c, 0x1a
};

const __flash uint8_t letterConversionTableDE[] = {
	0x04, 0x10, 0x14, 0x1a, 0x1d, 0x1c
};

const __flash uint8_t signConversionTableLenght = sizeof(signConversionTableEN);
const __flash uint8_t letterConversionTableLenght = sizeof(letterConversionTableEN);
