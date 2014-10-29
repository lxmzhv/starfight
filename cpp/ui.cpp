/* Game corners */

#include <stdio.h>
#include <gdk/gdkkeysyms.h>
#include <assert.h>

const char* GETTEXT_PACKAGE = "StarFight";
const char* LOCALEDIR       = "locale";

#include <ui.h>
#include <array.h>
#include <game.h>
#include <display.h>
//#include <strings.h>

// Variables

//GameOptions Options;
//P<Game> game;
//Field   PossibleTurns;

/*DisplOption DisplOpt = doDEFAULT;*/
//GameModes GameMode = gmHUMAN_AI;

bool Freeze = true;
bool Over = false;
//bool Interactive = true;
//Player Winner = pNO;
GdkPoint MouseDown, MouseMove;
int SelectedShip = -1;

GtkWidget    *Window = NULL;
GtkWidget    *DrawingArea = NULL;
GtkWidget    *RightBox = NULL;
GdkGC        *GC = NULL;
GtkStatusbar *StatusBar = NULL;
guint         StatusBarContextID = 0;
GtkWidget    *Menu = NULL;
GtkUIManager *UI = NULL;

const int MinTurnDelay = 10;
int TurnDelay    = 50; // milliseconds
int NewTurnDelay = TurnDelay;

static Array<GtkActionEntry>       MenuEntries;
static Array<GtkRadioActionEntry>  ColorEntries;

static gboolean Timer_cb( gpointer data );

// Functions

static void SetStatus( const char* msg )
{
  gtk_statusbar_push( StatusBar, StatusBarContextID, msg );
}

static void UpdateStatusBar()
{
  const int sz = 0x200;
  char buf[sz] = "";

  buf[sz-1] = 0;
  SetStatus( buf );
}

static void UpdateWindow()
{
  UpdateStatusBar();
  gdk_window_invalidate_rect( Window->window, &Window->allocation, TRUE );
}

static void Run()
{
  if( !Freeze )
    return;

  Freeze = false;
  TurnDelay = NewTurnDelay;
  g_timeout_add( TurnDelay, Timer_cb, NULL );
//  g_idle_add( Timer_cb, NULL );
  UpdateWindow();
}

static void Stop()
{
  if( Freeze )
    return;

  Freeze = true;
  UpdateWindow();
}

static void ToggleFreeze()
{
  Freeze ? Run() : Stop();
}

static void Init()
{
  Over = false;
  InitGame();
  MouseDown.x = MouseDown.x = -1;
  MouseMove.x = MouseMove.x = -1;
  SelectedShip = -1;
  UpdateWindow();
}

void NewGame()
{
  Init();
}

double ScreenToX( int x, GdkPoint sz )
{
  return (x - sz.x/2)/Scale + Center.x;
}

double ScreenToY( int y, GdkPoint sz )
{
  return (sz.y/2 - y)/Scale + Center.y;
}

int XToScreen( double x, GdkPoint sz )
{
  return sz.x/2 + int((x-Center.x)*Scale);
}

int YToScreen( double y, GdkPoint sz )
{
  return sz.y/2 - int((y-Center.y)*Scale);
}

static void MessageDialog( const char* message )
{
  GtkWidget* dialog = NULL;
  
  dialog = gtk_message_dialog_new(GTK_WINDOW(Window),
                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                  GTK_MESSAGE_INFO,
                                  GTK_BUTTONS_CLOSE,
                                  message);

  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

static void PostTurn()
{
  if( SelectedShip >= 0 )
  {
    Center.x = ships_x[SelectedShip];
    Center.y = ships_y[SelectedShip];
  }

  UpdateWindow();
}

static void AITurn()
{
  Step();
  PostTurn();
}

static gboolean Timer_cb( gpointer data )
{
  if( Freeze )
    return FALSE;

  if( NewTurnDelay != TurnDelay )
  {
    TurnDelay = NewTurnDelay;
    g_timeout_add( TurnDelay, Timer_cb, NULL );
    return FALSE;
  }

  AITurn();
  return TRUE;
}

static gboolean DrawField_cb( GtkWidget *widget, GdkEventExpose *event, gpointer )
{
  DrawField( widget, GC );
  return TRUE;
}

static void Preferences()
{
}

static void ZoomIn( double perc )
{
  Scale *= 1+perc;
}

static void ZoomOut( double perc )
{
  Scale *= 1-perc;
}

static gboolean KeyDown_cb( GtkWidget   *widget,
                            GdkEventKey *event,
                            gpointer )
{
  bool control = (event->state & GDK_CONTROL_MASK) != 0;
  bool shift   = (event->state & GDK_SHIFT_MASK) != 0;

  switch( event->keyval )
  {
    case GDK_equal:
    case GDK_plus:  ZoomIn(0.1);   break;
    case GDK_minus: ZoomOut(0.09); break;
    case GDK_space: ToggleFreeze();  break;
    case GDK_Page_Up:   NewTurnDelay = Max(MinTurnDelay, int(TurnDelay * 0.91)); break;
    case GDK_Page_Down: NewTurnDelay = TurnDelay + Max(10, TurnDelay/10);        break;
    case GDK_Up:
    case GDK_Down:
    case GDK_Left:
    case GDK_Right: return FALSE;
    case GDK_z: InitGame(); break;
    case GDK_ISO_Left_Tab:
    case GDK_Tab: SelectedShip = NextAliveShip(SelectedShip, !shift); break;
    default: return FALSE;
  }

  UpdateWindow();
  return TRUE;
}

static gboolean MouseDown_cb( GtkWidget *widget,
                              GdkEventButton *event,
                              gpointer )
{
  if( event->type != GDK_BUTTON_PRESS )
    return FALSE;

  switch( event->button )
  {
    case 1:
    {
      GdkPoint sz = { widget->allocation.width, widget->allocation.height };
      MouseDown.x = (int)event->x;
      MouseDown.y = (int)event->y;

      SelectedShip = FindShip( ScreenToX(MouseDown.x, sz), ScreenToY(MouseDown.y, sz) );

      return TRUE;
    }
    case 3:
    {
      return TRUE;
    }
  }

  return FALSE;
}

static void MoveDisplayCenter( GdkPoint from, GdkPoint to, GdkPoint sz )
{
  Center.x -= ScreenToX(to.x, sz) - ScreenToX(from.x, sz);
  Center.y -= ScreenToY(to.y, sz) - ScreenToY(from.y, sz);
}

static gboolean MouseMove_cb( GtkWidget *widget,
                              GdkEventMotion *event,
                              gpointer )
{
  if( event->state & GDK_BUTTON1_MASK )
  {
    GdkPoint sz = { widget->allocation.width, widget->allocation.height };
    MouseMove.x = (int)event->x;
    MouseMove.y = (int)event->y;
    MoveDisplayCenter( MouseDown, MouseMove, sz );
    MouseDown = MouseMove;
    UpdateWindow();
  }

  return FALSE;
}

static gboolean MouseUp_cb( GtkWidget *widget,
                            GdkEventButton *event,
                            gpointer)
{
  if( event->type != GDK_BUTTON_RELEASE )
    return FALSE;

/*  Point2i up;*/
  //up.x = (int)event->x;
  //up.y = (int)event->y;

  MouseDown.x = MouseDown.y = -1; 
  MouseMove.x = MouseMove.y = -1; 

  UpdateWindow();

  return FALSE;
}

static gboolean Scroll_cb( GtkWidget *widget,
                           GdkEventScroll *event,
                           gpointer)
{
  if( event->type != GDK_SCROLL )
    return FALSE;

  switch( event->direction )
  {
    case GDK_SCROLL_UP:   ZoomIn( 0.01 );  break;
    case GDK_SCROLL_DOWN: ZoomOut( 0.009 ); break;
    default: return FALSE;
  }

  UpdateWindow();

  return FALSE;
}

static gboolean Quit_cb()
{
  gtk_main_quit();
  return FALSE;
}

static void MenuQuit_cb( GtkAction *action )
{
  gtk_main_quit();
}

static void ActionInit( GtkActionEntry *action,
                         const gchar *name,
                         const gchar *stock_id,
                         const gchar *label,
                         const gchar *accelerator,
                         const gchar *tooltip,
                         GCallback callback )
{
  action->name        = name;
  action->stock_id    = stock_id;
  action->label       = label;
  action->accelerator = accelerator;
  action->tooltip     = tooltip;
  action->callback    = callback;
}

static void RadioActionInit( GtkRadioActionEntry *action,
                             const gchar *name,
                             const gchar *stock_id,
                             const gchar *label,
                             const gchar *accelerator,
                             const gchar *tooltip,
                             gint value )
{
  action->name        = name;
  action->stock_id    = stock_id;
  action->label       = label;
  action->accelerator = accelerator;
  action->tooltip     = tooltip;
  action->value       = value;
}

static void ToggleActionInit( GtkToggleActionEntry *action,
                              const gchar *name,
                              const gchar *stock_id,
                              const gchar *label,
                              const gchar *accelerator,
                              const gchar *tooltip,
                              GCallback callback,
                              gboolean is_active )
{
  action->name        = name;
  action->stock_id    = stock_id;
  action->label       = label;
  action->accelerator = accelerator;
  action->tooltip     = tooltip;
  action->callback    = callback;
  action->is_active   = is_active;
}

static void CreateMenuEntries()
{
  /* name, stock id, label, accelerator, tooltip, callback */
  ActionInit( MenuEntries.Append(1), "GameMenu",        0, _("_Game"), 0, 0, 0 );
  ActionInit( MenuEntries.Append(1), "ActionMenu",      0, _("_Action"), 0, 0, 0 );
  ActionInit( MenuEntries.Append(1), "DisplayMenu",     0, _("_Display"), 0, 0, 0 );
  ActionInit( MenuEntries.Append(1), "HelpMenu",        0, _("_Help"), 0, 0, 0 );
  ActionInit( MenuEntries.Append(1), "New",   GTK_STOCK_NEW,  _("_New"),  "<control>N", _("New game"), G_CALLBACK(Init) );
  ActionInit( MenuEntries.Append(1), "Quit",  GTK_STOCK_QUIT, _("_Quit"), "<control>Q", _("Quit"), G_CALLBACK(MenuQuit_cb) );
  /*ActionInit( MenuEntries.Append(1), "About", GTK_STOCK_ABOUT,_("_About"), NULL,        _("About"), G_CALLBACK(About) );*/
  /*ActionInit( MenuEntries.Append(1), "Rules", GTK_STOCK_HELP, _("_Rules"), NULL,        _("Rules"), G_CALLBACK(Rules) );*/
  ActionInit( MenuEntries.Append(1), "Pause", GTK_STOCK_MEDIA_PAUSE, _("_Pause"), "P",  _("Pause"), G_CALLBACK(ToggleFreeze) );
  ActionInit( MenuEntries.Append(1), "Preferences", GTK_STOCK_PREFERENCES, _("_Preferences"), "<control>P",  _("Preferences"), G_CALLBACK(Preferences) );
}

static const char *UIInfo = "\
<ui>\
  <menubar name='MenuBar'>\
    <menu action='GameMenu'>\
      <menuitem action='New'/>\
      <menuitem action='Preferences'/>\
      <separator/>\
      <menuitem action='Quit'/>\
    </menu>\
    <menu action='ActionMenu'>\
      <menuitem action='Pause'/>\
    </menu>\
    <menu action='DisplayMenu'>\
    </menu>\
    <menu action='HelpMenu'>\
      <menuitem action='Rules'/>\
      <menuitem action='About'/>\
    </menu>\
  </menubar>\
  <popup name='LocalMenu'>\
    <menuitem action='Pause'/>\
  </popup>\
  <toolbar  name='ToolBar'>\
    <toolitem action='New'/>\
    <separator/>\
    <toolitem action='Pause'/>\
    <separator/>\
    <toolitem action='Preferences'/>\
  </toolbar>\
</ui>";

static void AdjustWindowSize()
{
  GdkScreen* screen = gdk_screen_get_default();

  /* Get screen size, subtract panel size & windows decoration */
  gint width  = gdk_screen_get_width( screen )*95/100;
  gint height = gdk_screen_get_height( screen )*80/100;

  /* Subtract info area size */
  GdkPoint rb_size = { 0, 0 };
  gtk_widget_get_size_request( RightBox, &rb_size.x, &rb_size.y );
  width -= rb_size.x;

  /* Change field size */
  gtk_window_set_position( GTK_WINDOW(Window), GTK_WIN_POS_CENTER );
  gtk_widget_set_size_request( DrawingArea, width, height );
}

static void CreateMenu()
{
  if( UI != NULL )
    return;

  CreateMenuEntries();

  GtkActionGroup *actions = gtk_action_group_new( "Actions" );
  gtk_action_group_add_actions( actions, MenuEntries, MenuEntries.Size(), Window );

  UI = gtk_ui_manager_new();
  gtk_ui_manager_insert_action_group( UI, actions, 0 );

  GError *error = NULL;
  if( gtk_ui_manager_add_ui_from_string( UI, UIInfo, -1, &error ) )
    return;

  g_message( "Building menus failed: %s", error->message );
  g_error_free( error );
}

static void CreateDialog()
{
  GtkWidget *label = NULL,
            *statusbar = NULL,
            *vbox0 = NULL,
            *hbox1 = NULL,
            *menu_bar = NULL,
            *tool_bar = NULL;
  
  CreateMenu();

  /* Create widgets used by the application */

  Window      = gtk_window_new( GTK_WINDOW_TOPLEVEL );
  DrawingArea = gtk_drawing_area_new();
  statusbar   = gtk_statusbar_new();
  StatusBar   = GTK_STATUSBAR(statusbar);
  StatusBarContextID = gtk_statusbar_get_context_id( StatusBar, "" );
  vbox0       = gtk_vbox_new( FALSE, 0 );
  hbox1       = gtk_hbox_new( FALSE, 0 );
  RightBox    = gtk_vbox_new( FALSE, 0 );

  menu_bar = gtk_ui_manager_get_widget(UI, "/MenuBar");
  tool_bar = gtk_ui_manager_get_widget(UI, "/ToolBar");

  /* Lay the widgets on-screen */

  gtk_container_add( GTK_CONTAINER(Window), vbox0 );

  gtk_box_pack_start( GTK_BOX(vbox0), menu_bar, FALSE, FALSE, 0 );
  gtk_box_pack_start( GTK_BOX(vbox0), tool_bar, FALSE, FALSE, 0 );
  gtk_container_add( GTK_CONTAINER(vbox0), hbox1 );
  gtk_box_pack_end( GTK_BOX(vbox0), statusbar, FALSE, FALSE, 0 );

  gtk_container_add( GTK_CONTAINER( hbox1 ), DrawingArea );
  gtk_box_pack_end( GTK_BOX(hbox1), RightBox, FALSE, FALSE, 0 );

  /* Set properties */

  gtk_window_add_accel_group( GTK_WINDOW(Window), gtk_ui_manager_get_accel_group(UI) );
  gtk_window_set_title( GTK_WINDOW(Window), _("Star Fight") );
  gtk_widget_set_double_buffered( DrawingArea, TRUE );
  gtk_widget_set_events( DrawingArea, gtk_widget_get_events(DrawingArea)
                         | GDK_BUTTON_PRESS_MASK
                         | GDK_BUTTON_RELEASE_MASK
                         | GDK_POINTER_MOTION_MASK );

  /* Connect handlers */

  g_signal_connect( G_OBJECT( Window ),  "delete-event",
                    G_CALLBACK( Quit_cb ), NULL );

  g_signal_connect( G_OBJECT( DrawingArea ), "expose-event",
                    G_CALLBACK( DrawField_cb ), NULL );

  g_signal_connect( G_OBJECT( DrawingArea ), "button-press-event",
                    G_CALLBACK( MouseDown_cb ), NULL );

  g_signal_connect( G_OBJECT( DrawingArea ), "motion-notify-event",
                    G_CALLBACK( MouseMove_cb ), NULL );

  g_signal_connect( G_OBJECT( DrawingArea ), "button-release-event",
                    G_CALLBACK( MouseUp_cb ), NULL );

  g_signal_connect( G_OBJECT( DrawingArea ), "scroll-event",
                    G_CALLBACK( Scroll_cb ), NULL );

  g_signal_connect( G_OBJECT( Window ), "key-press-event",
                    G_CALLBACK( KeyDown_cb ), NULL );

  /* Other stuff  */

  AdjustWindowSize();
}

static void InitDrawing()
{
  if( GC == NULL )
    GC = gdk_gc_new( GDK_DRAWABLE( DrawingArea->window ) );
}

static void CleanupDrawing()
{
  g_object_unref( GC );
}

int main( int argc, char *argv[] )
{
  /* Set up i18n parameters */
  bindtextdomain( GETTEXT_PACKAGE, LOCALEDIR );
  bind_textdomain_codeset( GETTEXT_PACKAGE, "UTF-8" );
  textdomain( GETTEXT_PACKAGE );

  gtk_init( &argc, &argv );

  CreateDialog();

  gtk_widget_show_all( Window );
  gtk_window_set_focus( GTK_WINDOW(Window), NULL );

  /* Create new drawing context */
  InitDrawing();

  /* Init game */
  NewGame();

  /* Run */
  Run();
  gtk_main();

  /* Cleanup */
  CleanupDrawing();

  return 0;
}
