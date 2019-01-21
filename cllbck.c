
#include "intgrp.h"

int  gi_ind=0;						// index pour table tty dans le cas du RS232
int  gi_phs;						// phase de connection (non connecte = 0 ... access dev ok = 1 ... open dev ok = 2 ... dsr ok 3 ... cts ok 4 --> connecte = 5 :::: fin = 9)
int  gi_flg;						// etat processus == ar->flg
LS   *lstprt;						// contenu de controleur.brc mis en forme (port digital analogic timer)
LS   *lstbrc;						// contenu de controleur.brc (brochage)
char gr_dev[32];					// open port serie
char gr_snd[LNGSND+1];
char gr_rcp[LNGRCV+6];
int  gi_rcp;
char gr_rcv[LNGRCV+1];
int  gi_anl;						// analogique affichage dynamique (nombre a afficher)
char gr_anl[90];					// adc actif + correspondance portbit/n°adc
char gr_dgt[90];					// dgt revennu actif suite suppression adc
char gc_dgt[11][3] = {"", "", "", "", "", "", "c0", "c1", "c2", "d4", "b0"};
// broche uniquement analogique
char gr_brc[12][8][8];
// Broche reserve arduino --> disable (broche + io)
char gr_ard[12][8][8];
// l'affichage des broches
char gr_baf[12][8][32];
// l'aide des broches
char gr_bhl[12][8][32];
// l'aide des io
char gr_hlp[12][8][32];
int  gi_maj[SRT]; 			// tableau maj
char gr_cmd[SRT][LNGSND];	// tableau commande
int  gi_clr;			// flag memorisation fin init changement couleur (rouge) si changement combo
// memorisation tableau parametre = representation de ar->srt[][]
// mmysrt[.][] = ligne (ind) 1...B
// mmysrt[][.] = colonne parametre (ind sortie etat ... imn imx f )
struct _mmysrt
{
	GdkRGBA oclr;
	GdkRGBA nclr;
	char ovlr[17];
	char nvlr[17];
} mmysrt[SRT+1][16];
char gc_savett;			// sauvegarde valeur NO ETAT precedent
//unsigned char ar->srt[PRM+1][SRT+1];
char gc_brcspn[12] = {0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1};
unsigned char gc_12[3];

int  fi_sndmss(void);	// send message
int	 fi_sndi(char *);
int	 fi_sndam(char *);
int  fi_appcbt(char *, GtkComboBoxText *, char *);
int  fi_indcbt(char *, GtkComboBoxText *);
int  fi_indtxt(char *, GtkButton *, int);
int  fi_gstmcu(const char *);
int  fi_gstdev(const char *);
int  fi_gstprt(const char *);
void fv_majcmb(void);	// maj combo / memoire partagee
void fv_vnt(void);
gboolean cb_fin(gpointer);
gboolean cb_cnf(gpointer);
unsigned char *fp_ct12(unsigned char);

// Constructeur variable

void fv_cns_vrb(void)
{
	Wnd *wnd = &WND;
	char lr_dev[16];
	char *p, *lp_nommcu, *lp_nomlsn, *lp_nomdev, *lp_nomprt;
	gchar *txt;
	const gchar *lbl;
	int y, i, j, n, li_mmr;
	
	// computation going on...
	//while (gtk_events_pending())
	//	gtk_main_iteration();
	// ...computation continued

	lg.ntr = 5;
	lg.grv= LG_INF;
	lg_strcpy(lg.prj, "ardprm");
	lg.tll = 100000;
	lg.nmb = 3;
	lg_cns();

	gi_rcp = gi_clr = 0;
	gr_snd[0] = gr_rcv[0] = gr_rcp[0] = 0;
	gc_savett = 0;
	gc_12[2] = 0;
	
	lstprt = ls_cns();
	lstbrc = ls_cns();
	
	if ((li_mmr = shmget(CLEF, sizeof(Ar), 0660)) == -1)
	{
		printf("Erreur shmget() ardcmm n'a pas ete lance ...\n");
		lg_strcpy(lg.fnc, "main");
		lg.grv= LG_SYS;
		lg_strcpy(lg.mss, "shmget");
		lg_strcpy(lg.err, "Erreur");
		lg_ecr();
		exit(-1);
	}
	if ((gp_mmr = shmat(li_mmr, 0, 0)) == (void *) -1)																	// shmat(mmr, 0, SHM_RDONLY)
	{
		printf("Erreur shmat()\n");
		lg_strcpy(lg.fnc, "main");
		lg.grv= LG_SYS;
		lg_strcpy(lg.mss, "shmat");
		lg_strcpy(lg.err, "Erreur");
		lg_ecr();
		return;
	}
	ar = (Ar *) gp_mmr;
	
	// maj combo / memoire partagee
	
	fv_majcmb();

	for (y=0; y<SRT; y++)
	{
		gi_maj[y] = 0;
		sprintf(gr_cmd[y], "{x%c ", 'a'+y);
	}
	
	gi_clr = 1;
	gi_phs = 0;
	ar->cnf |= 1;
	ar->flg |= 1;
	g_timeout_add_seconds(1, cb_cnf, NULL);
}

gboolean cb_fin(gpointer dnn)
{
	if (gi_phs != 10)
		return(TRUE);
		
	if (gp_mmr != NULL)
		shmdt(gp_mmr);
	gtk_main_quit();
	return(FALSE);
}

// Destructeur variable

void fv_dst_vrb(void)
{
	ls_dst(lstprt);
	ls_dst(lstbrc);
	ar->flg &= ~1;
}

//
// Envoi commande
//
// retour: -1 erreur_send 1 erreur_receive 0 ok_send_receive

int fi_sndmss(void)
{
	int i;
	
	//printf("--> %s\n", gr_snd);
	for (i=0; (ar->env!=0)&&(i<NMBSND); i++)
	{
		usleep(2000);
	}
	if (ar->env == 0)
	{
		strcpy(ar->snd, gr_snd);
		ar->env |= 1;
		fv_trdmss(gr_snd);
	}
	
	return(0);
}

//
// rempli un combobox_text (pp_cbt) avec le contenu d'un fichier (pp_fch)
// selection d'un texte (pp_txt)
//
// retour: 0=ok 1=append_pasdeselection -1=erreur
//

int fi_appcbt(char *pp_fch, GtkComboBoxText *pp_cbt, char *pp_txt)
{
	FILE *lo;
	char *p, lr_lgn[80];
	int  id, ret;

	id = 0;
	ret = 1;
	if ((lo = fopen(pp_fch, "r")) == NULL)																				// fichier inconnu
		return(-1);
		
	gtk_combo_box_text_remove_all(pp_cbt);
	while (fgets(lr_lgn, sizeof(lr_lgn), lo) != NULL)
	{
		if ((p = strchr(lr_lgn, '\n')) != NULL)
			*p = 0;
		if ((p = strchr(lr_lgn, '\r')) != NULL)
			*p = 0;
		
		if (lr_lgn[0] != 0)
		{
			gtk_combo_box_text_append_text(pp_cbt, lr_lgn);
			if (pp_txt)
			{
				if (!strcmp(pp_txt, lr_lgn))
				{
					gtk_combo_box_set_active((GtkComboBox *) pp_cbt, id);
					ret = 0;
				}
			}
			id++;
		}
	}
	//if (!pp_txt)
	//{
	//	gtk_combo_box_set_active((GtkComboBox *) pp_cbt, 0);
	//	ret = 0;
	//}
		
	fclose(lo);
	
	return(ret);
}

//
// lit un fichier (pp_fch)
// et donne l'indice du texte actif du combobox
//
// retour: -1:pas_trouve 0-n=indice
//

int fi_indcbt(char *pp_fch, GtkComboBoxText *pp_cbt)
{
	FILE *lo;
	char *p, lr_lgn[80];
	int  id, ret;
	gchar *txt;

	if ((txt = gtk_combo_box_text_get_active_text(pp_cbt)) == NULL)
		return(-1);

	if ((lo = fopen(pp_fch, "r")) == NULL)																				// fichier inconnu
	{
		g_free(txt);
		return(-1);
	}
		
	id = 0;
	ret = 1;
	while (fgets(lr_lgn, sizeof(lr_lgn), lo) != NULL)
	{
		if ((p = strchr(lr_lgn, '\n')) != NULL)
			*p = 0;
		if ((p = strchr(lr_lgn, '\r')) != NULL)
			*p = 0;

		if (!strcmp(lr_lgn, txt))
		{
			ret = 0;
			break;
		}

		id++;
	}
		
	fclose(lo);
	g_free(txt);
	
	return(ret?-1:id);
}

//
// lit un fichier (pp_fch)
// et affiche le texte de la ligne pi_lgn
// dans le button pp_btt
//
// retour: -1:pas_trouve 0=ok
//

int fi_indtxt(char *pp_fch, GtkButton *pp_btt, int pi_lgn)
{
	FILE *lo;
	char *p, lr_lgn[80];
	int  id;


	if ((lo = fopen(pp_fch, "r")) == NULL)																				// fichier inconnu
		return(-1);
		
	id = 0;
	while (fgets(lr_lgn, sizeof(lr_lgn), lo) != NULL)
	{
		if ((p = strchr(lr_lgn, '\n')) != NULL)
			*p = 0;
		if ((p = strchr(lr_lgn, '\r')) != NULL)
			*p = 0;

		if (id == pi_lgn)
		{
			gtk_button_set_label(pp_btt, lr_lgn);
			break;
		}

		id++;
	}
		
	fclose(lo);
	
	return(0);
}


int fi_gstmcu(const char *pp_mcu)
{
	return(0);
}

int fi_gstdev(const char *pp_dev)
{
	sprintf(gr_dev, "/dev/%s", pp_dev);
	
	return(0);
}

int fi_gstprt(const char *pp_prt)
{
	return(0);
}

//
// Mise a jour combo en fonction de la memoire partagee
//

void fv_majcmb(void)
{
	Wnd *wnd = &WND;
	FILE *lo;
	char *d, *f, lr_nomfch[80], lr_lgn[10];
	int  v, p, s, t;
	// garni les combobox
	
	for (s=0; s<SRT; s++)
	{
		sprintf(lr_nomfch, "%s/index.lst", DSSFCH);
		fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][INDEX], NULL);
		gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][INDEX], s+1);
		
		sprintf(lr_nomfch, "%s/sortie.lst", DSSFCH);
		fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][SORTIE], NULL);
		gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][SORTIE], ar->srt[s][NSR]);
		
		sprintf(lr_nomfch, "%s/entree.lst", DSSFCH);
		fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][ENTREE], NULL);
		gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][ENTREE], ar->srt[s][NEN]);

		sprintf(lr_nomfch, "%s/capteur.lst", DSSFCH);
		fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][CAPTEUR], NULL);
		gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][CAPTEUR], ar->srt[s][NCP]);

		sprintf(lr_nomfch, "%s/fonction.lst", DSSFCH);
		fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][FONCTION], NULL);
		gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][FONCTION], 1);

		if (ar->srt[s][OTP] == OTP_NO)																					// pas de sortie
		{
			sprintf(lr_nomfch, "%s/etat.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][ETAT], "NO");
			//gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][ETAT],   0);
			
			sprintf(lr_nomfch, "%s/no_broche.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][BROCHE], "NO");
			//gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][BROCHE], 0);
			
			sprintf(lr_nomfch, "%s/no_omn.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][SMINI], "NO");
			//gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][SMINI],  0);
			
			sprintf(lr_nomfch, "%s/no_omx.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][SMAXI], "NO");
			//gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][SMAXI],  0);
		}
		if (ar->srt[s][OTP] == OTP_OFF)																					// sortie arret forcee
		{
			sprintf(lr_nomfch, "%s/etat.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][ETAT], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][ETAT], 1);

			sprintf(lr_nomfch, "%s/dgt_broche.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][BROCHE], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][BROCHE], ar->srt[s][OBR]+1);

			sprintf(lr_nomfch, "%s/off_omn.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][SMINI], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][SMINI], 0);

			sprintf(lr_nomfch, "%s/off_omx.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][SMAXI], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][SMAXI], 0);
		}

		if (ar->srt[s][OTP] == OTP_ON)																					// sortie marche forcee
		{
			sprintf(lr_nomfch, "%s/etat.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][ETAT], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][ETAT], 2);

			sprintf(lr_nomfch, "%s/dgt_broche.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][BROCHE], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][BROCHE], ar->srt[s][OBR]+1);

			sprintf(lr_nomfch, "%s/on_omn.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][SMINI], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][SMINI], 0);

			sprintf(lr_nomfch, "%s/on_omx.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][SMAXI], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][SMAXI], 0);
		}

		if (ar->srt[s][OTP] == OTP_DGT)																					// sortie numerique
		{
			sprintf(lr_nomfch, "%s/etat.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][ETAT], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][ETAT], 3);

			sprintf(lr_nomfch, "%s/dgt_broche.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][BROCHE], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][BROCHE], ar->srt[s][OBR]+1);

			sprintf(lr_nomfch, "%s/dgt_omn.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][SMINI], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][SMINI], (!ar->srt[s][OMN])?1:2);

			sprintf(lr_nomfch, "%s/dgt_omx.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][SMAXI], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][SMAXI], (!ar->srt[s][OMX])?1:2);
		}

		if (ar->srt[s][OTP] == OTP_PWM)																					// sortie analogique (pwm)
		{
			sprintf(lr_nomfch, "%s/etat.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][ETAT], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][ETAT], 4);

			sprintf(lr_nomfch, "%s/pwm_broche.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][BROCHE], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][BROCHE], gc_brcpwm[ar->srt[s][OBR]]);

			sprintf(lr_nomfch, "%s/pwm_omn.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][SMINI], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][SMINI], ar->srt[s][OMN]/10+1);

			sprintf(lr_nomfch, "%s/pwm_omx.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][SMAXI], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][SMAXI], ar->srt[s][OMX]/10+1);
		}

		if (ar->srt[s][ITP] == ITP_NO)																					// pas d'entree
		{
			sprintf(lr_nomfch, "%s/type.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][TYPE], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][TYPE],   0);

			sprintf(lr_nomfch, "%s/no_numero.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][NUMERO], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][NUMERO], 0);

			sprintf(lr_nomfch, "%s/no_imn.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][MMINI], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][MMINI],  0);

			sprintf(lr_nomfch, "%s/no_imx.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][MMAXI], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][MMAXI],  0);
		}

		if (ar->srt[s][ITP] == ITP_DGT)																					// entree numerique
		{
			sprintf(lr_nomfch, "%s/type.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][TYPE], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][TYPE], 1);

			sprintf(lr_nomfch, "%s/dgt_numero.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][NUMERO], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][NUMERO], ar->srt[s][IBR]+1);

			sprintf(lr_nomfch, "%s/dgt_imn.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][MMINI], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][MMINI], (!ar->srt[s][IMN])?1:2);

			sprintf(lr_nomfch, "%s/dgt_imx.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][MMAXI], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][MMAXI], (!ar->srt[s][IMX])?1:2);
		}

		if (ar->srt[s][ITP] == ITP_ANL)																					// entree analogique
		{
			sprintf(lr_nomfch, "%s/type.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][TYPE], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][TYPE], 2);
			
			sprintf(lr_nomfch, "%s/anl_numero.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][NUMERO], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][NUMERO], ar->srt[s][IBR]+1);

			sprintf(lr_nomfch, "%s/anl_imn.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][MMINI], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][MMINI], ar->srt[s][IMN]/40+1);

			sprintf(lr_nomfch, "%s/anl_imx.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][MMAXI], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][MMAXI], ar->srt[s][IMX]/40+1);
		}

		if (ar->srt[s][ITP] == ITP_OWR)																					// entree onewire (temperature)
		{
			sprintf(lr_nomfch, "%s/type.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][TYPE], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][TYPE], 3);

			sprintf(lr_nomfch, "%s/owr_numero.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][NUMERO], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][NUMERO], ar->srt[s][IBR]+1);

			sprintf(lr_nomfch, "%s/owr_imn.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][MMINI], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][MMINI], (ar->srt[s][IMN]>>2)/5+1);

			sprintf(lr_nomfch, "%s/owr_imx.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[s][MMAXI], NULL);
			gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[s][MMAXI], (ar->srt[s][IMX]>>2)/5+1);
		}
	}
}

//
// Gestion voyant etat connection
//

void fv_vnt(void)
{
	Wnd *wnd = &WND;

	switch (ar->cnn)
	{
		case 0:																										// non connecte
			wnd->pled = gtk_image_new_from_file("/var/local/icn/prouge.png");
			break;
		
		case 1:																										// access dev
			wnd->pled = gtk_image_new_from_file("/var/local/icn/pmagenta.png");
			break;

		case 2:																										// open
			wnd->pled = gtk_image_new_from_file("/var/local/icn/porange.png");
			break;

		case 3:																										// baud
			wnd->pled = gtk_image_new_from_file("/var/local/icn/pjaune.png");
			break;

		case 4:																										// dsr
			wnd->pled = gtk_image_new_from_file("/var/local/icn/pcyan.png");
			break;

		case 5:																										// dsr et cts
			wnd->pled = gtk_image_new_from_file("/var/local/icn/pbleu.png");
			break;

		case 6:																										// connecte (attente reception)
			wnd->pled = gtk_image_new_from_file("/var/local/icn/pvert.png");
			break;

		case 7:																										// pas utilise
		case 8:																										// pas utilise
			wnd->pled = gtk_image_new_from_file("/var/local/icn/pblanc.png");
			break;

		case 9:																										// pas utilise
			wnd->pled = gtk_image_new_from_file("/var/local/icn/pnoir.png");
			break;

		default:																									// arret ardcmm (-1)
			wnd->pled = gtk_image_new_from_file("/var/local/icn/pgris.png");
			break;
	}
	gtk_button_set_image((GtkButton *) wnd->bttcnn, wnd->pled);
}

// CallBack config 1S

gboolean cb_cnf(gpointer dnn)
{
	Wnd *wnd = &WND;
	//GdkRGBA color;
	char n[4], vlr[32], txt[80], mss[80];
	int  i, j, l, f, ret, li_nmb;
	int y, x;
	const char *pbc;
	const gchar *acc;																									// mode access du bouton
	const gchar *o_pbc;																									// nom du bouton
	const gchar *lbl;
	char *p, lr_nomfch[80];
	
	if (ar->cnn == 9)
		return(FALSE);

	for (y=0; y<SRT; y++)
	{
		while (ar->maj[y])
		{
			x = 0;
			i = 1;
			while (!(ar->maj[y] & i))
			{
				x++;
				i = 1 << x;
			}
if (gi_clr>0) printf("<~~ MAJ srt[%s][%s]\n", gr_srt[y], gr_prm[x]);		
			
			if (x == ITP)
			{
				gtk_widget_set_name(wnd->cprm[y][TYPE], "NOIR");
				gtk_widget_set_name(wnd->cprm[y][FONCTION], "NOIR");
			}
			if (x == IBR)
			{
				gtk_widget_set_name(wnd->cprm[y][NUMERO], "NOIR");
				gtk_widget_set_name(wnd->cprm[y][FONCTION], "NOIR");
			}
			if (x == OTP)
			{
				gtk_widget_set_name(wnd->cprm[y][ETAT], "NOIR");
				gtk_widget_set_name(wnd->cprm[y][FONCTION], "NOIR");
			}
			if (x == OBR)
			{
				gtk_widget_set_name(wnd->cprm[y][BROCHE], "NOIR");
				gtk_widget_set_name(wnd->cprm[y][FONCTION], "NOIR");
			}
			if (x == IMN)
			{
				if (ar->srt[y][ITP] == ITP_DGT)																					// entree numerique
				{
					gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[y][MMINI], (!ar->srt[y][IMN])?1:2);
				}
				if (ar->srt[y][ITP] == ITP_OWR)																					// entree onewire (temperature)
				{
					gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[y][MMINI], (ar->srt[y][IMN]>>2)/5+1);
				}
				if (ar->srt[y][ITP] == ITP_ANL)																					// entree analogique
				{
					gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[y][MMINI], ar->srt[y][IMN]/40+1);
				}
				gtk_widget_set_name(wnd->cprm[y][MMINI], "NOIR");
				gtk_widget_set_name(wnd->cprm[y][FONCTION], "NOIR");
			}
			if (x == IMX)
			{
				if (ar->srt[y][ITP] == ITP_DGT)																					// entree numerique
				{
					gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[y][MMAXI], (!ar->srt[y][IMX])?1:2);
				}
				if (ar->srt[y][ITP] == ITP_OWR)																					// entree onewire (temperature)
				{
					gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[y][MMAXI], (ar->srt[y][IMX]>>2)/5+1);
				}
				if (ar->srt[y][ITP] == ITP_ANL)																					// entree analogique
				{
					gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[y][MMAXI], ar->srt[y][IMX]/40+1);
				}
				gtk_widget_set_name(wnd->cprm[y][MMAXI], "NOIR");
				gtk_widget_set_name(wnd->cprm[y][FONCTION], "NOIR");
			}
			if (x == OMN)
			{
				if (ar->srt[y][OTP] == OTP_DGT)																					// entree numerique
				{
					gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[y][SMINI], (!ar->srt[y][OMN])?1:2);
				}
				if (ar->srt[y][OTP] == OTP_PWM)																					// entree analogique
				{
					gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[y][SMINI], ar->srt[y][OMN]/10+1);
				}
				gtk_widget_set_name(wnd->cprm[y][SMINI], "NOIR");
				gtk_widget_set_name(wnd->cprm[y][FONCTION], "NOIR");
			}
			if (x == OMX)
			{
				if (ar->srt[y][OTP] == OTP_DGT)																					// entree numerique
				{
					gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[y][SMAXI], (!ar->srt[y][OMX])?1:2);
				}
				if (ar->srt[y][OTP] == OTP_PWM)																					// entree analogique
				{
					gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[y][SMAXI], ar->srt[y][OMX]/10+1);
				}
				gtk_widget_set_name(wnd->cprm[y][SMAXI], "NOIR");
				gtk_widget_set_name(wnd->cprm[y][FONCTION], "NOIR");
			}
			if (x == NEN)
			{
				gtk_widget_set_name(wnd->cprm[y][ENTREE], "NOIR");
				gtk_widget_set_name(wnd->cprm[y][FONCTION], "NOIR");
			}
			if (x == NCP)
			{
				gtk_widget_set_name(wnd->cprm[y][CAPTEUR], "NOIR");
				gtk_widget_set_name(wnd->cprm[y][FONCTION], "NOIR");
			}
			if (x == NSR)
			{
				gtk_widget_set_name(wnd->cprm[y][SORTIE], "NOIR");
				gtk_widget_set_name(wnd->cprm[y][FONCTION], "NOIR");
			}
			
			ar->maj[y] &= ~i;
			if (x >= MSR)
				break;
		}
	}

	if (ar->cnf & 1)
	{
		sprintf(lr_nomfch, "%s/controleur.lst", DSSFCH);
		fi_indtxt(lr_nomfch, (GtkButton *) wnd->bttmcu, ar->crt);
		lbl = gtk_button_get_label((GtkButton *) wnd->bttmcu);
		fi_gstmcu(lbl);
		sprintf(lr_nomfch, "%s/liaison.lst", DSSFCH);
		fi_indtxt(lr_nomfch, (GtkButton *) wnd->bttlsn, ar->lsn);
		lbl = gtk_button_get_label((GtkButton *) wnd->bttlsn);
		if (!strcmp(lbl, "RS232"))
		{
			sprintf(vlr, "ttyS%d", ar->tty);
			gtk_button_set_label((GtkButton *) wnd->btttty, vlr);
		}
		if (!strcmp(lbl, "USB"))
		{
			sprintf(vlr, "ttyUSB%d", ar->tty);
			gtk_button_set_label((GtkButton *) wnd->btttty, vlr);
		}
		sprintf(lr_nomfch, "%s/port.lst", DSSFCH);
		fi_indtxt(lr_nomfch, (GtkButton *) wnd->bttprt, ar->prt);
		fv_vnt();
		ar->cnf &= ~1;
	}

	if (ar->flg != gi_flg)
	{
		gi_flg = ar->flg;

		if (gi_flg & 4)
		{
			gtk_image_set_from_file((GtkImage *) wnd->imgcmm, "/var/local/icn/ardcmm.png");
		}
		else
		{
			gtk_image_set_from_file((GtkImage *) wnd->imgcmm, "/var/local/icn/_ardcmm.png");
		}
		if (gi_flg & 1)
		{
			gtk_image_set_from_file((GtkImage *) wnd->imgprm, "/var/local/icn/ardprm.png");
		}
		else
		{
			gtk_image_set_from_file((GtkImage *) wnd->imgprm, "/var/local/icn/_ardprm.png");
		}
		if (gi_flg & 2)
		{
			gtk_image_set_from_file((GtkImage *) wnd->imgsrv, "/var/local/icn/ardsrv.png");
		}
		else
		{
			gtk_image_set_from_file((GtkImage *) wnd->imgsrv, "/var/local/icn/_ardsrv.png");
		}
		if (gi_flg & 8)
		{
			gtk_image_set_from_file((GtkImage *) wnd->imgsml, "/var/local/icn/ardsml.png");
		}
		else
		{
			gtk_image_set_from_file((GtkImage *) wnd->imgsml, "/var/local/icn/_ardsml.png");
		}
	}
			
	return(TRUE);
}

//
// Le contenu d'un comboboxtext a change
//

void on_cbtprm_changed(GtkComboBox *obj, gpointer dnn)
{
	Wnd *wnd = &WND;
	//GdkRGBA color;
	char vlr[8], lr_nomfch[80];
	int  i, x, y, ind;
	gchar *txt;
	//const gchar *name;
	
	if ((txt = gtk_combo_box_text_get_active_text((GtkComboBoxText *) obj)) == NULL)
		return;
	
	//name = gtk_widget_get_name((GtkWidget *) obj);
	for (y=0; y<SRT; y++)
	{
		for (x=0; x<MAJ; x++)
		{
			if (obj == (GtkComboBox *) wnd->cprm[y][x])
				break;
		}
		if (x != MAJ)
			break;
	}
	//printf("gi_clr=%d txt=%s x=%d y=%d %s\n", gi_clr, txt, x, y, ((x==MAJ)||(y==SRT))?"pas trouve":"ok");
	gi_maj[y] |= 1 << x;

	//printf("gi_clr=%d txt=%s x=%d y=%d gi_maj[%d]=$%x\n", gi_clr, txt, x, y, y, gi_maj[y]);

	if (gi_clr == 1)
	{
		//mmysrt[gi_clr].lgn = y;
		//mmysrt[gi_clr].cln = x;
		strcpy(mmysrt[y][x].ovlr, mmysrt[y][x].nvlr);
		strcpy(mmysrt[y][x].nvlr, txt);
		//printf("changed %s-->%s\n", mmysrt[y][x].ovlr, mmysrt[y][x].nvlr);

		if (x == INDEX)																									// index
		{
			if (!strcmp(txt, "NO"))
			{
				gtk_widget_set_name(wnd->cprm[y][0], "CLRNO");
				for (x=1; x<MAJ; x++)
				{
					gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[y][x], 0);
					gtk_widget_set_name(wnd->cprm[y][x], "CLRNO");
				}
			}
			else
				gtk_widget_set_name(wnd->cprm[y][0], "NOIR");
		}
		else
		{
			if (!strcmp(txt, "NO"))
				gtk_widget_set_name(wnd->cprm[y][x], "CLRNO");
			else if (mmysrt[y][x].ovlr == mmysrt[y][x].nvlr)
				gtk_widget_set_name(wnd->cprm[y][x], "NOIR");
			else
				gtk_widget_set_name(wnd->cprm[y][x], "CLRROM");
		}
	}
	//else
	//	gtk_widget_set_name(wnd->cprm[y][x], "NOIR");

	if (x == ETAT)																										// etat
	{
		//printf("maj ar->srt[%d][%d]\n", TIO, j);
		/*
		switch (*(txt+1))
		{
			case '|':																									// 0|1
			case 'N':																									// on
			case 'F':																									// off
				ar->srt[TIO][j] &= ~1;
				break;
			case 'W':																									// pwm
				ar->srt[TIO][j] |= 1;
				break;
		}
		*/
		
		if (!strcmp(txt, "PWM"))																						// pwm
		{
			//gdk_rgba_parse(&color, "#666AAA000000");
			//gtk_widget_override_color(wnd->cprm[j][INDEX], GTK_STATE_FLAG_NORMAL, &color);
			//gtk_widget_override_color(wnd->cprm[j][SORTIE], GTK_STATE_FLAG_NORMAL, &color);
			//gtk_widget_override_color(wnd->cprm[j][ETAT], GTK_STATE_FLAG_NORMAL, &color);
			//gtk_widget_override_color(wnd->cprm[j][BROCHE], GTK_STATE_FLAG_NORMAL, &color);
			//gtk_widget_override_color(wnd->cprm[j][SMINI], GTK_STATE_FLAG_NORMAL, &color);
			//gtk_widget_override_color(wnd->cprm[j][SMAXI], GTK_STATE_FLAG_NORMAL, &color);

			gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[y][BROCHE]);
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[y][BROCHE], "NO");
			for (i=0; i<SRT; i++)
			{
				if ((ar->srt[i][OBR] == 3) && (y != i))
					break;
			}
			if (i == SRT)
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[y][BROCHE], "D3");
			for (i=0; i<SRT; i++)
			{
				if ((ar->srt[i][OBR] == 5) && (y != i))
					break;
			}
			if (i == SRT)
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[y][BROCHE], "D5");
			for (i=0; i<SRT; i++)
			{
				if ((ar->srt[i][OBR] == 6) && (y != i))
					break;
			}
			if (i == SRT)
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[y][BROCHE], "D6");
			for (i=0; i<SRT; i++)
			{
				if ((ar->srt[i][OBR] == 9) && (y != i))
					break;
			}
			if (i == SRT)
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[y][BROCHE], "D9");
			for (i=0; i<SRT; i++)
			{
				if ((ar->srt[i][OBR] == 10) && (y != i))
					break;
			}
			if (i == SRT)
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[y][BROCHE], "D10");
			for (i=0; i<SRT; i++)
			{
				if ((ar->srt[i][OBR] == 11) && (y != i))
					break;
			}
			if (i == SRT)
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[y][BROCHE], "D11");

			gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[y][SMINI]);
			sprintf(lr_nomfch, "%s/pwm_omn.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][SMINI], NULL);

			gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[y][SMAXI]);
			sprintf(lr_nomfch, "%s/pwm_omx.lst", DSSFCH);
			fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][SMAXI], NULL);
		}
		else																											// no off on dgt 
		{
			gtk_combo_box_text_remove_all((GtkComboBoxText *) wnd->cprm[y][BROCHE]);
			gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[y][BROCHE], "NO");
			for (i=0; i<SRT; i++)
			{
				if ((ar->srt[i][OBR] == 2) && (y != i))
					break;
			}
			if (i == SRT)
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[y][BROCHE], "D2");
			for (i=0; i<SRT; i++)
			{
				if ((ar->srt[i][OBR] == 3) && (y != i))
					break;
			}
			if (i == SRT)
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[y][BROCHE], "D3");
			for (i=0; i<SRT; i++)
			{
				if ((ar->srt[i][OBR] == 4) && (y != i))
					break;
			}
			if (i == SRT)
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[y][BROCHE], "D4");
			for (i=0; i<SRT; i++)
			{
				if ((ar->srt[i][OBR] == 5) && (y != i))
					break;
			}
			if (i == SRT)
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[y][BROCHE], "D5");
			for (i=0; i<SRT; i++)
			{
				if ((ar->srt[i][OBR] == 6) && (y != i))
					break;
			}
			if (i == SRT)
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[y][BROCHE], "D6");
			/*
			for (i=0; i<SRT; i++)
			{
				if ((ar->srt[i][OBR] == 7) && (y != i))
					break;
			}
			if (i == SRT)
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[y][BROCHE], "D7");
			for (i=0; i<SRT; i++)
			{
				if ((ar->srt[i][OBR] == 8) && (y != i))
					break;
			}
			if (i == SRT)
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[y][BROCHE], "D8");
			*/
			for (i=0; i<SRT; i++)
			{
				if ((ar->srt[i][OBR] == 9) && (y != i))
					break;
			}
			if (i == SRT)
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[y][BROCHE], "D9");
			for (i=0; i<SRT; i++)
			{
				if ((ar->srt[i][OBR] == 10) && (y != i))
					break;
			}
			if (i == SRT)
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[y][BROCHE], "D10");
			for (i=0; i<SRT; i++)
			{
				if ((ar->srt[i][OBR] == 11) && (y != i))
					break;
			}
			if (i == SRT)
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[y][BROCHE], "D11");
			for (i=0; i<SRT; i++)
			{
				if ((ar->srt[i][OBR] == 18) && (y != i))
					break;
			}
			if (i == SRT)
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[y][BROCHE], "A4");
			for (i=0; i<SRT; i++)
			{
				if ((ar->srt[i][OBR] == 19) && (y != i))
					break;
			}
			if (i == SRT)
				gtk_combo_box_text_append_text((GtkComboBoxText *) wnd->cprm[y][BROCHE], "A5");

			if (!strcmp(txt, "DGT"))																					// etat DGT
			{
				sprintf(lr_nomfch, "%s/dgt_omn.lst", DSSFCH);
				fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][SMINI], NULL);
				sprintf(lr_nomfch, "%s/dgt_omx.lst", DSSFCH);
				fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][SMAXI], NULL);
			}
			else if (!strcmp(txt, "OFF"))																				// etat OFF
			{
				sprintf(lr_nomfch, "%s/no_omn.lst", DSSFCH);
				fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][SMINI], NULL);
				sprintf(lr_nomfch, "%s/no_omx.lst", DSSFCH);
				fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][SMAXI], NULL);
			}
			else if (!strcmp(txt, "ON"))																				// etat ON
			{
				sprintf(lr_nomfch, "%s/no_omn.lst", DSSFCH);
				fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][SMINI], NULL);
				sprintf(lr_nomfch, "%s/no_omx.lst", DSSFCH);
				fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][SMAXI], NULL);
			}
			else if (!strcmp(txt, "NO"))																				// etat NO
			{
				sprintf(lr_nomfch, "%s/no_omn.lst", DSSFCH);
				fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][SMINI], NULL);
				sprintf(lr_nomfch, "%s/no_omx.lst", DSSFCH);
				fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][SMAXI], NULL);
				/*				
				for (x=1; x<MAJ; x++)
				{
					if (x == ETAT)
						continue;
					
					gtk_combo_box_set_active((GtkComboBox *) wnd->cprm[y][x], 0);
					//gtk_widget_override_color(wnd->cprm[j][x], GTK_STATE_FLAG_NORMAL, &color);
				}
				*/
			}
		}
		//gtk_widget_override_color(wnd->cprm[j][ETAT], GTK_STATE_FLAG_NORMAL, &color);
	}

	if (x == TYPE)																										// itp type
	{
		
		switch (*txt)
		{
			case 'D':																									// dgt
				//ar->srt[TIO][y] &= ~6;
				sprintf(lr_nomfch, "%s/dgt_numero.lst", DSSFCH);
				fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][NUMERO], NULL);
				sprintf(lr_nomfch, "%s/dgt_imn.lst", DSSFCH);
				fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][MMINI], NULL);
				sprintf(lr_nomfch, "%s/dgt_imx.lst", DSSFCH);
				fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][MMAXI], NULL);
				break;
				
			case 'A':																									// anl
				//ar->srt[TIO][y] &= ~4;
				//ar->srt[TIO][y] |= 2;
				sprintf(lr_nomfch, "%s/anl_numero.lst", DSSFCH);
				fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][NUMERO], NULL);
				sprintf(lr_nomfch, "%s/anl_imn.lst", DSSFCH);
				fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][MMINI], NULL);
				sprintf(lr_nomfch, "%s/anl_imx.lst", DSSFCH);
				fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][MMAXI], NULL);
				break;
				
			case 'O':																									// owr
				//ar->srt[TIO][y] &= ~2;
				//ar->srt[TIO][y] |= 4;
				sprintf(lr_nomfch, "%s/owr_numero.lst", DSSFCH);
				fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][NUMERO], NULL);
				sprintf(lr_nomfch, "%s/owr_imn.lst", DSSFCH);
				fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][MMINI], NULL);
				sprintf(lr_nomfch, "%s/owr_imx.lst", DSSFCH);
				fi_appcbt(lr_nomfch, (GtkComboBoxText *) wnd->cprm[y][MMAXI], NULL);
				break;
		}
	}

	if (x == FONCTION)																									// fonction
	{
		if (*txt == 'M')																								// maj
		{
			//gdk_rgba_parse(&color, "#CCC000CCC000");
			//gtk_widget_override_color(wnd->cprm[j][FONCTION], GTK_STATE_FLAG_NORMAL, &color);
			//gtk_widget_override_color(wnd->cprm[j][MAJ], GTK_STATE_FLAG_NORMAL, &color);
			gtk_button_set_label((GtkButton *) wnd->cprm[y][MAJ], "MAJ");
			sprintf(gr_cmd[y], "}x%c ", 'a'+y);
			gtk_widget_set_name(wnd->cprm[y][MAJ], "CLRMAJ");//vlr);
		}
		else if (*txt == 'L')																							// lct
		{
			//gdk_rgba_parse(&color, "#000AAA000000");
			//gtk_widget_override_color(wnd->cprm[j][FONCTION], GTK_STATE_FLAG_NORMAL, &color);
			//gtk_widget_override_color(wnd->cprm[j][MAJ], GTK_STATE_FLAG_NORMAL, &color);
			gtk_button_set_label((GtkButton *) wnd->cprm[y][MAJ], "LCT");
			sprintf(gr_cmd[y], "{x%c ", 'a'+y);
			gtk_widget_set_name(wnd->cprm[y][MAJ], "CLRLCT");//vlr);
		}
		else if (*txt == 'R')																							// rom
		{
			//gdk_rgba_parse(&color, "#CCC000000000");
			//gtk_widget_override_color(wnd->cprm[j][FONCTION], GTK_STATE_FLAG_NORMAL, &color);
			//gtk_widget_override_color(wnd->cprm[j][MAJ], GTK_STATE_FLAG_NORMAL, &color);
			gtk_button_set_label((GtkButton *) wnd->cprm[y][MAJ], "ROM");
			sprintf(gr_cmd[y], "|x%c ", 'a'+y);
			gtk_widget_set_name(wnd->cprm[y][MAJ], "CLRROM");//vlr);
		}
		else																											// no
		{
			//gdk_rgba_parse(&color, "#FFFAAA000000");
			//gtk_widget_override_color(wnd->cprm[j][FONCTION], GTK_STATE_FLAG_NORMAL, &color);
			//gtk_widget_override_color(wnd->cprm[j][MAJ], GTK_STATE_FLAG_NORMAL, &color);
			gtk_button_set_label((GtkButton *) wnd->cprm[y][MAJ], "NO");
			sprintf(gr_cmd[y], "ux%c ", 'a'+y);
			gtk_widget_set_name(wnd->cprm[y][MAJ], "CLRNO");
		}
	}

	//if (!strcmp(txt, "NO"))
	//	gc_savett = 0;
	//else
		gc_savett = 1;
	/*
	if (gi_clr)
	{
		//gi_clr++;
		if ((GtkWidget *) obj != wnd->cprm[j][FONCTION])
		{
			gdk_rgba_parse(&color, "#DDD000000000");
			gtk_widget_override_color((GtkWidget *) obj, GTK_STATE_FLAG_NORMAL, &color);
		}
	}
	*/
	//if (gi_phs == 5)
	//	memcpy(gp_mmr, srt, sizeof(srt));	// recopie memoire partagee dans variable locale
		
	g_free(txt);
}

//
// boutton lct/maj/rom click
//

void on_bttmaj_clicked(GtkButton *obj, gpointer dnn)
{
	Wnd *wnd = &WND;
	const gchar *texte;
	int  i, j, k, y, ind;
	char *txt, vlr[16], prt[8], lr_nomfch[80], tblv[]=" kefghijabcd";
	unsigned char c;
	
	for (y=0; y<SRT; y++)																								// selection sortie -> y
	{
		if (obj == (GtkButton *) wnd->cprm[y][MAJ])
			break;
	}
	
//printf("gr_cmd[%d]=%s\n", y, gr_cmd[y]);
	switch (gr_cmd[y][1])
	{
		case 'x':
printf("bttmaj(1) gr_cmd[%d]=<%s> gi_maj[%d]=$%x\n", y, gr_cmd[y], y, gi_maj[y]);
			
			k = 0;
			while (gi_maj[y] & 0xFFF)
			{
				strcpy(gr_snd, gr_cmd[y]);
				
				i = gi_maj[y];
				j = 0;
				while ((i & 1) == 0)
				{
					i >>= 1;
					j++;
				}
				gr_snd[3] = tblv[j];
printf("bttmaj(2) i=$%x j=%d tblv[%d]=%c mmy[%d][%d]=%s\n", i, j, j, tblv[j], y, j, mmysrt[y][j].nvlr);
	
				if (gr_snd[0] != '{')
				{
					switch (gr_snd[3])
					{
						case 'a':																						// ITP
							if (!strcmp(mmysrt[y][j].nvlr, "NO"))
								gr_snd[4] = 'a' + ITP_NO;
							if (!strcmp(mmysrt[y][j].nvlr, "DGT"))
								gr_snd[4] = 'a' + ITP_DGT;
							if (!strcmp(mmysrt[y][j].nvlr, "ANL"))
								gr_snd[4] = 'a' + ITP_ANL;
							if (!strcmp(mmysrt[y][j].nvlr, "OWR"))
								gr_snd[4] = 'a' + ITP_OWR;
							gr_snd[5] = 0;
							fi_sndmss();
							break;

						case 'b':																						// IBR
							if (!strcmp(mmysrt[y][j].nvlr, "NO"))
								k = 0x300;
							else
								k = atoi(&mmysrt[y][j].nvlr[1]);
							k &= 0x3FF; 
							prt[0] = ' ' + (k >> 6);
							prt[1] = ' ' + (k & 0x3F);
							prt[2] = 0;
							strcat(gr_snd, prt);
							fi_sndmss();
							break;

						case 'c':																						// IMN
						case 'd':																						// IMX
							if (strcmp(mmysrt[y][j].nvlr, "ON") == 0)
							{
								gr_snd[4] = '`';
								gr_snd[5] = 0;
								fi_sndmss();
							}
							else if (strcmp(mmysrt[y][j].nvlr, "OFF") == 0)
							{
								gr_snd[4] = '~';
								gr_snd[5] = 0;
								fi_sndmss();
							}
							else
							{
								k = atoi(mmysrt[y][j].nvlr);
								if (ar->srt[y][ITP] == ITP_OWR)															// temperarure * 4 (10bits 20,25°C)
									k <<= 2;
								k &= 0x3FF; 
								prt[0] = ' ' + (k >> 6);
								prt[1] = ' ' + (k & 0x3F);
								prt[2] = 0;
								strcat(gr_snd, prt);
								fi_sndmss();
							}
							break;

						case 'e':																						// OTP
							if (!strcmp(mmysrt[y][j].nvlr, "NO"))
								gr_snd[4] = 'a' + OTP_NO;
							if (!strcmp(mmysrt[y][j].nvlr, "OFF"))
								gr_snd[4] = 'a' + OTP_OFF;
							if (!strcmp(mmysrt[y][j].nvlr, "ON"))
								gr_snd[4] = 'a' + OTP_ON;
							if (!strcmp(mmysrt[y][j].nvlr, "DGT"))
								gr_snd[4] = 'a' + OTP_DGT;
							if (!strcmp(mmysrt[y][j].nvlr, "PWM"))
								gr_snd[4] = 'a' + OTP_PWM;
							gr_snd[5] = 0;
							fi_sndmss();
							break;

						case 'f':																						// OBR
							if (!strcmp(mmysrt[y][j].nvlr, "NO"))
								k = 0x300;
							else
							{
								k = atoi(&mmysrt[y][j].nvlr[1]);
								if (mmysrt[y][j].nvlr[0] == 'A')
									k += 14;
							}
							k &= 0x3FF; 
							prt[0] = ' ' + (k >> 6);
							prt[1] = ' ' + (k & 0x3F);
							prt[2] = 0;
							strcat(gr_snd, prt);
							fi_sndmss();
							break;

						case 'g':																						// OMN
						case 'h':																						// OMX
							if (strcmp(mmysrt[y][j].nvlr, "ON") == 0)
							{
								gr_snd[4] = '`';
								gr_snd[5] = 0;
								fi_sndmss();
							}
							else if (strcmp(mmysrt[y][j].nvlr, "OFF") == 0)
							{
								gr_snd[4] = '~';
								gr_snd[5] = 0;
								fi_sndmss();
							}
							else
							{
								k = atoi(mmysrt[y][j].nvlr);
								k &= 0x3FF; 
								prt[0] = ' ' + (k >> 6);
								prt[1] = ' ' + (k & 0x3F);
								prt[2] = 0;
								strcat(gr_snd, prt);
								fi_sndmss();
							}
							break;

						case ' ':																						// ind
							k = 0xFFFF;
							break;

						default:																						// nen ncp nsr
							k = atoi(mmysrt[y][j].nvlr);
							k &= 0x3FF; 
							prt[0] = ' ' + (k >> 6);
							prt[1] = ' ' + (k & 0x3F);
							prt[2] = 0;
							strcat(gr_snd, prt);
							fi_sndmss();
							break;
					}
					/*
					if (k != 0xFFFF)
					{
						k &= 0x3FF; 
						prt[0] = ' ' + (k >> 6);
						prt[1] = ' ' + (k & 0x3F);
						prt[2] = 0;
						strcat(gr_snd, prt);
						fi_sndmss();
					}
					*/
				}
				else
				{
					fi_sndmss();
				}
				
				if (gr_snd[3] == ' ') gi_maj[y] &= ~1;
				if (gr_snd[3] == 'k') gi_maj[y] &= ~2;
				if (gr_snd[3] == 'e') gi_maj[y] &= ~4;
				if (gr_snd[3] == 'f') gi_maj[y] &= ~8;
				if (gr_snd[3] == 'g') gi_maj[y] &= ~0x10;
				if (gr_snd[3] == 'h') gi_maj[y] &= ~0x20;
				if (gr_snd[3] == 'i') gi_maj[y] &= ~0x40;
				if (gr_snd[3] == 'j') gi_maj[y] &= ~0x80;
				if (gr_snd[3] == 'a') gi_maj[y] &= ~0x100;
				if (gr_snd[3] == 'b') gi_maj[y] &= ~0x200;
				if (gr_snd[3] == 'c') gi_maj[y] &= ~0x400;
				if (gr_snd[3] == 'd') gi_maj[y] &= ~0x800;
				//gi_clr = 1;
			}
			//gi_clr = 1;
			break;

		default:
			printf("Commande '%c' non traitee !!!\n", gr_cmd[y][1]);
			break;
	}
	
	return;
}

unsigned char *fp_ct12(unsigned char pc)
{
	gc_12[1] = ' ' + (pc & 0x3F);
	gc_12[0] = ' ' + (pc >> 6);
	
	return(gc_12);
}
/*
unsigned char *fp_ct12(unsigned int pi)
{
	gc_12[1] = ' ' + (pi & 0x3F);
	gc_12[0] = ' ' + ((pi >> 6) & 0x3F);
	
	return(gc_12);
}
*/

