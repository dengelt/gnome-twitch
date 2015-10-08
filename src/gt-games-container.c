#include "gt-games-container.h"
#include "gt-games-container-child.h"
#include "gt-channels-view.h"
#include "gt-win.h"

typedef struct
{
    GtkWidget* games_flow;
    GtkWidget* load_revealer;
} GtGamesContainerPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(GtGamesContainer, gt_games_container, GTK_TYPE_BOX)

enum 
{
    PROP_0,
    NUM_PROPS
};

enum
{
    SIG_GAME_ACTIVATED,
    
    NUM_SIGS
};

static GParamSpec* props[NUM_PROPS];

static guint sigs[NUM_SIGS];

GtGamesContainer*
gt_games_container_new(void)
{
    return g_object_new(GT_TYPE_GAMES_CONTAINER, 
                        NULL);
}

static void
show_load_spinner(GtGamesContainer* self, gboolean show)
{
    GtGamesContainerPrivate* priv = gt_games_container_get_instance_private(self);

    gtk_revealer_set_reveal_child(GTK_REVEALER(priv->load_revealer), show);
}

static void
append_games(GtGamesContainer* self, GList* games)
{
    GtGamesContainerPrivate* priv = gt_games_container_get_instance_private(self);

    for (GList* l = games; l != NULL; l = l->next)
    {
        GtGame* game = GT_GAME(l->data);

        GtGamesContainerChild* child = gt_games_container_child_new(game);
        gtk_widget_show_all(GTK_WIDGET(child));
        gtk_container_add(GTK_CONTAINER(priv->games_flow), GTK_WIDGET(child));
    }
}

static GtkFlowBox*
get_games_flow(GtGamesContainer* self)
{
    GtGamesContainerPrivate* priv = gt_games_container_get_instance_private(self);

    return GTK_FLOW_BOX(priv->games_flow);
}

static void
edge_reached_cb(GtkScrolledWindow* scroll,
                GtkPositionType pos,
                gpointer udata)
{
    GtGamesContainer* self = GT_GAMES_CONTAINER(udata);
    GtGamesContainerPrivate* priv = gt_games_container_get_instance_private(self);

    if (pos == GTK_POS_BOTTOM)
    {
        GT_GAMES_CONTAINER_GET_CLASS(self)->bottom_edge_reached(self);
    }
}

static void
child_activated_cb(GtkFlowBox* flow,
                   GtkFlowBoxChild* child,
                   gpointer udata)
{
    GtGamesContainer* self = GT_GAMES_CONTAINER(udata);
    GtGamesContainerChild* con = GT_GAMES_CONTAINER_CHILD(child);
    GtGame* game;
    GtWin* win = GT_WIN_TOPLEVEL(flow);

    gt_games_container_child_hide_overlay(con);

    g_object_get(con, "game", &game, NULL);

    g_signal_emit(self, sigs[SIG_GAME_ACTIVATED], 0, game);

    /* gt_channels_view_set_game(gt_win_get_channels_view(win), */
                              /* game); */

    /* gt_win_browse_channels_view(win); */

    g_object_unref(game);
}

static void
finalize(GObject* object)
{
    GtGamesContainer* self = (GtGamesContainer*) object;
    GtGamesContainerPrivate* priv = gt_games_container_get_instance_private(self);

    G_OBJECT_CLASS(gt_games_container_parent_class)->finalize(object);
}

static void
get_property (GObject*    obj,
              guint       prop,
              GValue*     val,
              GParamSpec* pspec)
{
    GtGamesContainer* self = GT_GAMES_CONTAINER(obj);
    GtGamesContainerPrivate* priv = gt_games_container_get_instance_private(self);

    switch (prop)
    {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop, pspec);
    }
}

static void
set_property(GObject*      obj,
             guint         prop,
             const GValue* val,
             GParamSpec*   pspec)
{
    GtGamesContainer* self = GT_GAMES_CONTAINER(obj);
    GtGamesContainerPrivate* priv = gt_games_container_get_instance_private(self);

    switch (prop)
    {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop, pspec);
    }
}

static void
gt_games_container_class_init(GtGamesContainerClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);

    object_class->finalize = finalize;
    object_class->get_property = get_property;
    object_class->set_property = set_property;

    klass->show_load_spinner = show_load_spinner;
    klass->append_games = append_games;
    klass->get_games_flow = get_games_flow;

    sigs[SIG_GAME_ACTIVATED] = g_signal_new("game-activated",
                                            GT_TYPE_GAMES_CONTAINER,
                                            G_SIGNAL_RUN_LAST,
                                            0, NULL, NULL,
                                            g_cclosure_marshal_VOID__OBJECT,
                                            G_TYPE_NONE,
                                            1, GT_TYPE_GAME);

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(klass), "/com/gnome-twitch/ui/gt-games-container.ui");

    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(klass), GtGamesContainer, games_flow);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(klass), GtGamesContainer, load_revealer);

    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(klass), edge_reached_cb);
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(klass), child_activated_cb);
}

static void
gt_games_container_init(GtGamesContainer* self)
{
    gtk_widget_init_template(GTK_WIDGET(self));
}

void
gt_games_container_refresh(GtGamesContainer* self)
{
    GtGamesContainerPrivate* priv = gt_games_container_get_instance_private(self);

    GT_GAMES_CONTAINER_GET_CLASS(self)->refresh(self);
}

void
gt_games_container_set_filter_query(GtGamesContainer* self, const gchar* query)
{
    GT_GAMES_CONTAINER_GET_CLASS(self)->filter(self, query);
}
