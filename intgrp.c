
// nom=INTGRP
// version=0.0
// quoi=Interface graphique
// qui=GNRGRP

// Include

#include "intgrp.h"

// Define

// Declaration fonction

void aff_fenetre(void);
void create_wnd(void);
void show_wnd(void);
void hide_wnd(void);

void fv_cns_vrb(void);
void fv_dst_vrb(void);

// Declaration callback

void on_wnd_destroy(GtkWindow *, gpointer);
void on_cbtprm_changed(GtkComboBox *, gpointer);
void on_bttmaj_clicked(GtkButton *, gpointer);

// Variable globale

Wnd WND;

// Creation fenetre

void create_wnd(void)
{
	Wnd *wnd = &WND;
	GtkCssProvider *provider;
	GdkDisplay *display;
	GdkScreen *screen;
	GFile *file;
	char t[32];
	int x, y;
	
	wnd->wnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow *) wnd->wnd, " Parametrage carte NANO");
	gtk_window_set_icon_from_file((GtkWindow *) wnd->wnd, "/var/local/icn/ardprm.png", NULL);
	gtk_window_set_default_size((GtkWindow *) wnd->wnd, 910, 440);
	//gtk_window_set_position((GtkWindow *) wnd->wnd, GTK_WIN_POS_CENTER_ALWAYS);

	wnd->box0 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_add((GtkContainer *) wnd->wnd, wnd->box0);

	wnd->frmsst = gtk_frame_new(NULL);
	gtk_widget_set_margin_start(wnd->frmsst, 5);
	gtk_widget_set_margin_end(wnd->frmsst, 5);
	//gtk_widget_set_margin_top(wnd->frmsst, 5);
	//gtk_widget_set_margin_bottom(wnd->frmsst, 5);
	gtk_box_pack_start((GtkBox *) wnd->box0, wnd->frmsst, FALSE, TRUE, 5);

	wnd->lblsst = gtk_label_new(" Systeme ");
	//gtk_label_set_markup((GtkLabel *) wnd->lblsst, "<span foreground=\"#222299\"><b> Systeme </b></span>");
	gtk_frame_set_label_widget((GtkFrame *) wnd->frmsst, wnd->lblsst);

	wnd->box3 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add((GtkContainer *) wnd->frmsst, wnd->box3);

	wnd->frmprc = gtk_frame_new(NULL);
	gtk_widget_set_margin_start(wnd->frmprc, 5);
	gtk_widget_set_margin_end(wnd->frmprc, 5);
	//gtk_widget_set_margin_top(wnd->frmprc, 5);
	gtk_widget_set_margin_bottom(wnd->frmprc, 5);
	gtk_widget_set_name(wnd->frmprc, "frmprc");
	gtk_box_pack_start((GtkBox *) wnd->box3, wnd->frmprc, FALSE, TRUE, 5);

	wnd->lblprc = gtk_label_new(" Processus ");
	//gtk_label_set_markup((GtkLabel *) wnd->lblprc, "<span foreground=\"#222299\"> Processus </span>");
	gtk_frame_set_label_widget((GtkFrame *) wnd->frmprc, wnd->lblprc);

	wnd->box5 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_widget_set_margin_start(wnd->box5, 5);
	gtk_widget_set_margin_end(wnd->box5, 5);
	//gtk_widget_set_margin_top(wnd->box5, 5);
	gtk_widget_set_margin_bottom(wnd->box5, 5);
	gtk_container_add((GtkContainer *) wnd->frmprc, wnd->box5);

	wnd->imgcmm = gtk_image_new_from_file("/var/local/icn/_ardprm.png");
	gtk_box_pack_start((GtkBox *) wnd->box5, wnd->imgcmm, TRUE, TRUE, 5);
	wnd->imgprm = gtk_image_new_from_file("/var/local/icn/ardprm.png");
	gtk_box_pack_start((GtkBox *) wnd->box5, wnd->imgprm, TRUE, TRUE, 5);
	wnd->imgsrv = gtk_image_new_from_file("/var/local/icn/_ardprm.png");
	gtk_box_pack_start((GtkBox *) wnd->box5, wnd->imgsrv, TRUE, TRUE, 5);
	wnd->imgsml = gtk_image_new_from_file("/var/local/icn/_ardprm.png");
	gtk_box_pack_start((GtkBox *) wnd->box5, wnd->imgsml, TRUE, TRUE, 5);

	wnd->frmcrt = gtk_frame_new(NULL);
	gtk_widget_set_margin_start(wnd->frmcrt, 5);
	gtk_widget_set_margin_end(wnd->frmcrt, 5);
	//gtk_widget_set_margin_top(wnd->frmcrt, 5);
	gtk_widget_set_margin_bottom(wnd->frmcrt, 5);
	gtk_widget_set_name(wnd->frmcrt, "frmcrt");
	gtk_box_pack_end((GtkBox *) wnd->box3, wnd->frmcrt, FALSE, TRUE, 5);

	wnd->lblcrt = gtk_label_new(" Carte ");
	//gtk_label_set_markup((GtkLabel *) wnd->lblcrt, "<span foreground=\"#222299\"> Carte </span>");
	gtk_frame_set_label_widget((GtkFrame *) wnd->frmcrt, wnd->lblcrt);

	wnd->box4 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_widget_set_margin_start(wnd->box4, 5);
	gtk_widget_set_margin_end(wnd->box4, 5);
	//gtk_widget_set_margin_top(wnd->box4, 5);
	gtk_widget_set_margin_bottom(wnd->box4, 5);
	gtk_container_add((GtkContainer *) wnd->frmcrt, wnd->box4);

	wnd->bttmcu = gtk_button_new_with_label("");
	gtk_box_pack_start((GtkBox *) wnd->box4, wnd->bttmcu, TRUE, TRUE, 5);

	wnd->bttlsn = gtk_button_new_with_label("");
	gtk_box_pack_start((GtkBox *) wnd->box4, wnd->bttlsn, TRUE, TRUE, 5);

	wnd->btttty = gtk_button_new_with_label("");
	gtk_box_pack_start((GtkBox *) wnd->box4, wnd->btttty, TRUE, TRUE, 5);

	wnd->bttprt = gtk_button_new_with_label("");
	gtk_box_pack_start((GtkBox *) wnd->box4, wnd->bttprt, TRUE, TRUE, 5);

	wnd->bttcnn = gtk_button_new();
	wnd->pled = gtk_image_new_from_file("/var/local/icn/pgris.png");
	gtk_button_set_image((GtkButton *) wnd->bttcnn, wnd->pled);
	gtk_button_set_image_position((GtkButton *) wnd->bttcnn, GTK_POS_RIGHT);
	gtk_button_set_always_show_image((GtkButton *) wnd->bttcnn, TRUE);
	gtk_widget_set_name(wnd->bttcnn, "g");
	gtk_box_pack_end((GtkBox *) wnd->box4, wnd->bttcnn, TRUE, TRUE, 5);

	wnd->frmprm = gtk_frame_new(NULL);
	gtk_widget_set_margin_start(wnd->frmprm, 5);
	gtk_widget_set_margin_end(wnd->frmprm, 5);
	//gtk_widget_set_margin_top(wnd->frmprm, 5);
	//gtk_widget_set_margin_bottom(wnd->frmprm, 5);
	gtk_box_pack_end((GtkBox *) wnd->box0, wnd->frmprm, FALSE, TRUE, 5);

	wnd->lblprm = gtk_label_new(" Parametrage ");
	//gtk_label_set_markup((GtkLabel *) wnd->lblprm, "<span foreground=\"#222299\"><b> Parametrage </b></span>");
	gtk_frame_set_label_widget((GtkFrame *) wnd->frmprm, wnd->lblprm);

	wnd->grdprm = gtk_grid_new();
	gtk_widget_set_margin_start(wnd->grdprm, 5);
	gtk_widget_set_margin_end(wnd->grdprm, 5);
	//gtk_widget_set_margin_top(wnd->grdprm, 5);
	gtk_widget_set_margin_bottom(wnd->grdprm, 5);
	gtk_grid_set_column_homogeneous((GtkGrid *) wnd->grdprm, FALSE);
	//gtk_grid_set_column_spacing((GtkGrid *) wnd->grdprm, 7);
	gtk_container_add((GtkContainer *) wnd->frmprm, wnd->grdprm);

	wnd->lprm[INDEX] = gtk_label_new("Ind");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->lprm[INDEX], INDEX, 0, 1, 1);
	wnd->lprm[SORTIE] = gtk_label_new("Sortie");
	//gtk_label_set_markup((GtkLabel *) wnd->lprm[SORTIE], "<span foreground=\"#00AA66\">Sortie</span>");
	gtk_widget_set_name(wnd->lprm[SORTIE], "CLRSRT");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->lprm[SORTIE], SORTIE, 0, 1, 1);
	wnd->lprm[ETAT] = gtk_label_new("Etat");
	//gtk_label_set_markup((GtkLabel *) wnd->lprm[ETAT], "<span foreground=\"#00AA66\">Etat</span>");
	gtk_widget_set_name(wnd->lprm[ETAT], "CLRSRT");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->lprm[ETAT], ETAT, 0, 1, 1);
	wnd->lprm[BROCHE] = gtk_label_new("Broche");
	//gtk_label_set_markup((GtkLabel *) wnd->lprm[BROCHE], "<span foreground=\"#00AA66\">Broche</span>");
	gtk_widget_set_name(wnd->lprm[BROCHE], "CLRSRT");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->lprm[BROCHE], BROCHE, 0, 1, 1);
	wnd->lprm[SMINI] = gtk_label_new("mini");
	//gtk_label_set_markup((GtkLabel *) wnd->lprm[SMINI], "<span foreground=\"#00AA66\">mini</span>");
	gtk_widget_set_name(wnd->lprm[SMINI], "CLRSRT");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->lprm[SMINI], SMINI, 0, 1, 1);
	wnd->lprm[SMAXI] = gtk_label_new("MAXI");
	//gtk_label_set_markup((GtkLabel *) wnd->lprm[SMAXI], "<span foreground=\"#00AA66\">MAXI</span>");
	gtk_widget_set_name(wnd->lprm[SMAXI], "CLRSRT");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->lprm[SMAXI], SMAXI, 0, 1, 1);
	wnd->lprm[ENTREE] = gtk_label_new("Entree");
	//gtk_label_set_markup((GtkLabel *) wnd->lprm[ENTREE], "<span foreground=\"#00AAFF\">Entree</span>");
	gtk_widget_set_name(wnd->lprm[ENTREE], "CLRENT");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->lprm[ENTREE], ENTREE, 0, 1, 1);
	wnd->lprm[CAPTEUR] = gtk_label_new("Capteur");
	//gtk_label_set_markup((GtkLabel *) wnd->lprm[CAPTEUR], "<span foreground=\"#0066AA\">Capteur</span>");
	gtk_widget_set_name(wnd->lprm[CAPTEUR], "CLRENT");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->lprm[CAPTEUR], CAPTEUR, 0, 1, 1);
	wnd->lprm[TYPE] = gtk_label_new("Type");
	//gtk_label_set_markup((GtkLabel *) wnd->lprm[TYPE], "<span foreground=\"#0066AA\">Type</span>");
	gtk_widget_set_name(wnd->lprm[TYPE], "CLRENT");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->lprm[TYPE], TYPE, 0, 1, 1);
	wnd->lprm[NUMERO] = gtk_label_new("Numero");
	//gtk_label_set_markup((GtkLabel *) wnd->lprm[NUMERO], "<span foreground=\"#0066AA\">Numero</span>");
	gtk_widget_set_name(wnd->lprm[NUMERO], "CLRENT");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->lprm[NUMERO], NUMERO, 0, 1, 1);
	wnd->lprm[MMINI] = gtk_label_new("mini");
	//gtk_label_set_markup((GtkLabel *) wnd->lprm[MMINI], "<span foreground=\"#0066AA\">mini</span>");
	gtk_widget_set_name(wnd->lprm[MMINI], "CLRENT");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->lprm[MMINI], MMINI, 0, 1, 1);
	wnd->lprm[MMAXI] = gtk_label_new("MAXI");
	//gtk_label_set_markup((GtkLabel *) wnd->lprm[MMAXI], "<span foreground=\"#0066AA\">MAXI</span>");
	gtk_widget_set_name(wnd->lprm[MMAXI], "CLRENT");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->lprm[MMAXI], MMAXI, 0, 1, 1);
	wnd->lprm[FONCTION] = gtk_label_new("F");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->lprm[FONCTION], FONCTION, 0, 1, 1);

	wnd->bprm[INDEX] = gtk_label_new("");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->bprm[INDEX], INDEX, 1, 1, 1);
	wnd->bprm[SORTIE] = gtk_label_new("k=NSR");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->bprm[SORTIE], SORTIE, 1, 1, 1);
	wnd->bprm[ETAT] = gtk_label_new("e=OTP");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->bprm[ETAT], ETAT, 1, 1, 1);
	wnd->bprm[BROCHE] = gtk_label_new("f=OBR");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->bprm[BROCHE], BROCHE, 1, 1, 1);
	wnd->bprm[SMINI] = gtk_label_new("g=OMN");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->bprm[SMINI], SMINI, 1, 1, 1);
	wnd->bprm[SMAXI] = gtk_label_new("h=OMX");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->bprm[SMAXI], SMAXI, 1, 1, 1);
	wnd->bprm[ENTREE] = gtk_label_new("i=NEN");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->bprm[ENTREE], ENTREE, 1, 1, 1);
	wnd->bprm[CAPTEUR] = gtk_label_new("j=NCP");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->bprm[CAPTEUR], CAPTEUR, 1, 1, 1);
	wnd->bprm[TYPE] = gtk_label_new("a=ITP");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->bprm[TYPE], TYPE, 1, 1, 1);
	wnd->bprm[NUMERO] = gtk_label_new("b=IBR");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->bprm[NUMERO], NUMERO, 1, 1, 1);
	wnd->bprm[MMINI] = gtk_label_new("c=IMN");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->bprm[MMINI], MMINI, 1, 1, 1);
	wnd->bprm[MMAXI] = gtk_label_new("d=IMX");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->bprm[MMAXI], MMAXI, 1, 1, 1);
	wnd->bprm[FONCTION] = gtk_label_new("");
	gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->bprm[FONCTION], FONCTION, 1, 1, 1);

	for (y=0; y<SRT; y++)
	{
		wnd->cprm[y][INDEX] = gtk_combo_box_text_new();
		sprintf(t, "%c%c", 'a'+INDEX, 'a'+y);
		gtk_widget_set_name(wnd->cprm[y][INDEX], t);
		gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->cprm[y][INDEX], INDEX, y+2, 1, 1);
		wnd->cprm[y][SORTIE] = gtk_combo_box_text_new();
		sprintf(t, "%c%c", 'a'+SORTIE, 'a'+y);
		gtk_widget_set_name(wnd->cprm[y][SORTIE], t);
		gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->cprm[y][SORTIE], SORTIE, y+2, 1, 1);
		wnd->cprm[y][ETAT] = gtk_combo_box_text_new();
		sprintf(t, "%c%c", 'a'+ETAT, 'a'+y);
		gtk_widget_set_name(wnd->cprm[y][ETAT], t);
		gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->cprm[y][ETAT], ETAT, y+2, 1, 1);
		wnd->cprm[y][BROCHE] = gtk_combo_box_text_new();
		sprintf(t, "%c%c", 'a'+BROCHE, 'a'+y);
		gtk_widget_set_name(wnd->cprm[y][BROCHE], t);
		gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->cprm[y][BROCHE], BROCHE, y+2, 1, 1);
		wnd->cprm[y][SMINI] = gtk_combo_box_text_new();
		sprintf(t, "%c%c", 'a'+SMINI, 'a'+y);
		gtk_widget_set_name(wnd->cprm[y][SMINI], t);
		gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->cprm[y][SMINI], SMINI, y+2, 1, 1);
		wnd->cprm[y][SMAXI] = gtk_combo_box_text_new();
		sprintf(t, "%c%c", 'a'+SMAXI, 'a'+y);
		gtk_widget_set_name(wnd->cprm[y][SMAXI], t);
		gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->cprm[y][SMAXI], SMAXI, y+2, 1, 1);
		wnd->cprm[y][ENTREE] = gtk_combo_box_text_new();
		sprintf(t, "%c%c", 'a'+ENTREE, 'a'+y);
		gtk_widget_set_name(wnd->cprm[y][ENTREE], t);
		gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->cprm[y][ENTREE], ENTREE, y+2, 1, 1);
		wnd->cprm[y][CAPTEUR] = gtk_combo_box_text_new();
		sprintf(t, "%c%c", 'a'+CAPTEUR, 'a'+y);
		gtk_widget_set_name(wnd->cprm[y][CAPTEUR], t);
		gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->cprm[y][CAPTEUR], CAPTEUR, y+2, 1, 1);
		wnd->cprm[y][TYPE] = gtk_combo_box_text_new();
		sprintf(t, "%c%c", 'a'+TYPE, 'a'+y);
		gtk_widget_set_name(wnd->cprm[y][TYPE], t);
		gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->cprm[y][TYPE], TYPE, y+2, 1, 1);
		wnd->cprm[y][NUMERO] = gtk_combo_box_text_new();
		sprintf(t, "%c%c", 'a'+NUMERO, 'a'+y);
		gtk_widget_set_name(wnd->cprm[y][NUMERO], t);
		gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->cprm[y][NUMERO], NUMERO, y+2, 1, 1);
		wnd->cprm[y][MMINI] = gtk_combo_box_text_new();
		sprintf(t, "%c%c", 'a'+MMINI, 'a'+y);
		gtk_widget_set_name(wnd->cprm[y][MMINI], t);
		gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->cprm[y][MMINI], MMINI, y+2, 1, 1);
		wnd->cprm[y][MMAXI] = gtk_combo_box_text_new();
		sprintf(t, "%c%c", 'a'+MMAXI, 'a'+y);
		gtk_widget_set_name(wnd->cprm[y][MMAXI], t);
		gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->cprm[y][MMAXI], MMAXI, y+2, 1, 1);
		wnd->cprm[y][FONCTION] = gtk_combo_box_text_new();
		sprintf(t, "%c%c", 'a'+FONCTION, 'a'+y);
		gtk_widget_set_name(wnd->cprm[y][FONCTION], t);
		gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->cprm[y][FONCTION], FONCTION, y+2, 1, 1);

		wnd->cprm[y][MAJ] = gtk_button_new_with_label("LCT");
		//sprintf(t, "{x%c", 'a'+y);
		gtk_widget_set_name(wnd->cprm[y][MAJ], "CLRLCT");
		gtk_grid_attach((GtkGrid *) wnd->grdprm, wnd->cprm[y][MAJ], MAJ, y+2, 1, 1);
	}
	
	provider = gtk_css_provider_new();
	display = gdk_display_get_default();
	screen = gdk_display_get_default_screen(display);                                                                                  
	gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
	file = g_file_new_for_path("/var/local/cnf/ardprm.css");
	gtk_css_provider_load_from_file(GTK_CSS_PROVIDER (provider), file, NULL);
/*
GtkComboBox GtkBox {
	background-color: white;
	color: blue;
}
 
GtkComboBox GtkCellView {
	background-color: white;
	color: blue;
}
*/
/*
	gtk_css_provider_load_from_data (GTK_CSS_PROVIDER (provider),
		                    " GtkWindow"
		                    "{\n"                          
		                    "   background-color: #AAAAAA;\n"
		                    "}\n"
		                    " GtkFrame"
		                    "{\n"                          
		                    "   background-color: #CCCCCC;\n"
		                    "   color: #222299;\n"
		                    "}\n"
		                    " GtkFrame#frmprc, GtkFrame#frmcrt"
		                    "{\n"                          
		                    "   background-color: #DDDDDD;\n"
		                    "   color: #222299;\n"
		                    "}\n"
		                    " GtkButton#CLRLCT"
		                    "{\n"
		                    "   color: #00AA00;\n"                                       
		                    "}\n"
		                    " GtkButton#CLRMAJ"
		                    "{\n"
		                    "   color: #CC00CC;\n"                                       
		                    "}\n"
		                    " GtkButton#CLRROM"
		                    "{\n"
		                    "   color: #DD0000;\n"                                       
		                    "}\n"
		                    " GtkButton#CLRNO"
		                    "{\n"
		                    "   color: #FFDD22;\n"                                       
		                    "}\n"
		                    " GtkComboBox#CLRNO GtkButton"
		                    "{\n"
		                    "   color: #FFAA00;\n"                                       
		                    "}\n"
		                    , -1, NULL);
*/
	g_object_unref (provider);
	
	g_signal_connect((GtkWindow *) wnd->wnd, "destroy", (GCallback) on_wnd_destroy, NULL);
	for (y=0; y<SRT; y++)
	{
		g_signal_connect((GtkWindow *) wnd->cprm[y][INDEX],	  "changed", (GCallback) on_cbtprm_changed, NULL);
		g_signal_connect((GtkWindow *) wnd->cprm[y][SORTIE],  "changed", (GCallback) on_cbtprm_changed, NULL);
		g_signal_connect((GtkWindow *) wnd->cprm[y][ETAT],	  "changed", (GCallback) on_cbtprm_changed, NULL);
		g_signal_connect((GtkWindow *) wnd->cprm[y][BROCHE],  "changed", (GCallback) on_cbtprm_changed, NULL);
		g_signal_connect((GtkWindow *) wnd->cprm[y][SMINI],	  "changed", (GCallback) on_cbtprm_changed, NULL);
		g_signal_connect((GtkWindow *) wnd->cprm[y][SMAXI],	  "changed", (GCallback) on_cbtprm_changed, NULL);
		g_signal_connect((GtkWindow *) wnd->cprm[y][ENTREE],  "changed", (GCallback) on_cbtprm_changed, NULL);
		g_signal_connect((GtkWindow *) wnd->cprm[y][CAPTEUR], "changed", (GCallback) on_cbtprm_changed, NULL);
		g_signal_connect((GtkWindow *) wnd->cprm[y][TYPE],	  "changed", (GCallback) on_cbtprm_changed, NULL);
		g_signal_connect((GtkWindow *) wnd->cprm[y][NUMERO],  "changed", (GCallback) on_cbtprm_changed, NULL);
		g_signal_connect((GtkWindow *) wnd->cprm[y][MMINI],	  "changed", (GCallback) on_cbtprm_changed, NULL);
		g_signal_connect((GtkWindow *) wnd->cprm[y][MMAXI],	  "changed", (GCallback) on_cbtprm_changed, NULL);
		g_signal_connect((GtkWindow *) wnd->cprm[y][FONCTION],"changed", (GCallback) on_cbtprm_changed, NULL);
		g_signal_connect((GtkWindow *) wnd->cprm[y][MAJ],	  "clicked", (GCallback) on_bttmaj_clicked, NULL);
	}
}

// Affichage Fenetre

void show_wnd(void)
{
	Wnd *wnd = &WND;

	gtk_widget_show_all(wnd->wnd);
}

// Cache fenetre

void hide_wnd(void)
{
	Wnd *wnd = &WND;

	gtk_widget_hide(wnd->wnd);
}

// Quitte le programme

void on_wnd_destroy(GtkWindow *obj, gpointer dnn)
{
	//gi_phs = 9;
	fv_dst_vrb();
	gtk_main_quit();
}

//
// Affichage fenetre unique
//
// Appele par main.c
//

void aff_fenetre(void)
{
	create_wnd();

	fv_cns_vrb();

	show_wnd();
}
