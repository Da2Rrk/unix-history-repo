#ifndef TARGET
TARGET NOT DEFINED !!!
#endif
#if TARGET!=VAX
Target= TARGET OUT OF RANGE!!
#endif

#ifndef FAMILY
FAMILY NOT DEFINED!!!
#endif
#if FAMILY!=PCC && FAMILY!=DMR
Family = FAMILY OUT OF RANGE
#endif

#define SDB 1

#define TYLENG	TYLONG

#define TYINT	TYLONG
#define SZADDR	4
#define SZSHORT	2
#define SZINT	4

#define SZLONG	4
#define SZLENG	SZLONG

#define ALIADDR SZADDR
#define ALISHORT SZSHORT
#define ALILONG 4
#define ALIDOUBLE 4
#define ALIINT	ALILONG
#define ALILENG	ALILONG

#define AUTOREG 13
#define ARGREG 12
#define CARGREG 12
#define ARGOFFSET 4
#define SAVESPACE 40


#define FUDGEOFFSET 1
#define BITSPERCHAR 8
#define XL 16

#define USETEXT	".text"
#define USECONST	".data\t0"
#define USEBSS	".data\t1"
#define USEINIT	".data\t2"

#define BLANKCOMMON "_BLNK_"

#define LABELFMT "%s:\n"

#define MAXREGVAR 5
#define TYIREG TYLONG
#define MSKIREG  (M(TYSHORT)|M(TYLONG))

#define MAXINT	2147483647
#define MININT  ((-2147483647) - 1)

#define MAXBYTE	255
#define BYTESIZE	8

#define SMALLVAR(x) ((x) < 512)
