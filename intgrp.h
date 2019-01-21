
// nom=INTGRP
// version=0.0
// quoi=Extern pour main()...
// qui=GNRGRP

#ifndef INTGRP_H
#define INTGRP_H 1

#include <gtk/gtk.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "bblar.h"
#include "bblio.h"
#include "bbllg.h"
#include "bblht.h"

#ifndef NMBSND
#define NMBSND 5
#endif

#ifndef INDEX
#define INDEX		0
#endif
#ifndef SORTIE
#define SORTIE		1
#endif
#ifndef ETAT
#define ETAT		2
#endif
#ifndef BROCHE
#define BROCHE		3
#endif
#ifndef SMINI
#define SMINI		4
#endif
#ifndef SMAXI
#define SMAXI		5
#endif
#ifndef ENTREE
#define ENTREE		6
#endif
#ifndef CAPTEUR
#define CAPTEUR		7
#endif
#ifndef TYPE
#define TYPE		8
#endif
#ifndef NUMERO
#define NUMERO		9
#endif
#ifndef MMINI
#define MMINI		10
#endif
#ifndef MMAXI
#define MMAXI		11
#endif
#ifndef FONCTION
#define FONCTION	12
#endif
#ifndef MAJ
#define MAJ			13
#endif
#ifndef END
#define END			14
#endif

// nom du fichier config
#ifndef NOMCNF
#define NOMCNF "/var/local/cnf/ardprm"
#endif

extern void aff_fenetre(void);
extern void create_wnd(void);
extern void show_wnd(void);
extern void hide_wnd(void);

extern void on_cbtmcu_changed(GtkComboBox *, gpointer);
extern void on_cbtlsn_changed(GtkComboBox *, gpointer);
extern void on_cbtprt_changed(GtkComboBox *, gpointer);
extern void on_bdgt_clicked(GtkButton *, gpointer);
extern void on_bbrc_clicked(GtkButton *, gpointer);
extern void on_cbtprm_changed(GtkComboBox *, gpointer);
extern void on_cbtprm_popup(GtkComboBox *, gpointer);
extern void on_bttmaj_clicked(GtkButton *, gpointer);

#ifndef Wnd
typedef struct _wnd
{
	GtkWidget *wnd;
	GtkWidget *box0;
	GtkWidget *box1;
	GtkWidget *box2;
	GtkWidget *frmsst;
	GtkWidget *lblsst;
	GtkWidget *frmprc;
	GtkWidget *lblprc;
	GtkWidget *box5;
	GtkWidget *imgcmm;
	GtkWidget *imgprm;
	GtkWidget *imgsrv;
	GtkWidget *imgsml;
	GtkWidget *frmcrt;
	GtkWidget *lblcrt;
	GtkWidget *box4;
	GtkWidget *bttmcu;
	GtkWidget *bttlsn;
	GtkWidget *btttty;
	GtkWidget *bttprt;
	GtkWidget *bttcnn;
	GtkWidget *box3;
	GtkWidget *frmbrc;
	GtkWidget *grdbrc;
	GtkWidget *lbrcd[8];
	GtkWidget *lbrcp[12];
	GtkWidget *bbrc[12][8];
	GtkWidget *lblbrc;
	GtkWidget *frmtbl;
	GtkWidget *grdtbl;
	GtkWidget *ltbl[END];
	GtkWidget *etbl[END];
	GtkWidget *btbl[END];
	GtkWidget *lbltbl;
	GtkWidget *frmprm;
	GtkWidget *grdprm;
	GtkWidget *lprm[END];
	GtkWidget *bprm[END];
	GtkWidget *cprm[SRT][END];
	GtkWidget *lblprm;
	GtkWidget *pled;
	GtkEntryBuffer *ebf_mode;
	GtkEntryBuffer *ebf_capteur;
	GtkEntryBuffer *ebf_adresse;
} Wnd;
#endif

extern Wnd WND;
extern int gi_phs;
#endif
