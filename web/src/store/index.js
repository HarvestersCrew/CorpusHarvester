import Vue from "vue";
import Vuex from "vuex";
import router from "../router";

Vue.use(Vuex);

export default new Vuex.Store({
  state: {
    home_page: "Files",
    redirect_page: undefined,
    socket: {
      url: undefined,
      is_connected: false,
      connecting: false,
      error: false
    },
    first_init_done: false,

    logs: {
      unread: 0,
      messages: []
    },
    apis: undefined
  },
  mutations: {
    SOCKET_ONOPEN(state, event) {
      Vue.prototype.$socket = event.currentTarget;
      state.socket.url = event.currentTarget.url;
      localStorage.harvester_socket = state.socket.url;
      state.socket.is_connected = true;
      state.socket.connecting = false;
      state.socket.error = false;
      router.push({
        name:
          state.redirect_page !== undefined
            ? state.redirect_page
            : state.home_page
      });
    },
    SOCKET_ONCLOSE(state) {
      state.socket.is_connected = false;
      state.socket.connecting = false;
      state.socket.error = true;
      state.socket.url = undefined;
    },
    SOCKET_ONERROR(state, event) {
      console.error(state, event);
    },
    // default handler called for all methods
    SOCKET_ONMESSAGE(state, obj) {
      if (obj.type !== undefined) {
        switch (obj.type) {
          case "get_apis":
            state.apis = obj.data;
            break;

          case "logger":
            state.logs.messages.push(obj.data.message);
            state.logs.unread++;
            break;

          default:
            break;
        }
      }
    },

    set_socket_connecting(state, boolval) {
      state.socket.connecting = boolval;
    },

    set_first_init_done(state, boolval) {
      state.first_init_done = boolval;
    },

    set_redirect_page(state, val) {
      state.redirect_page = val;
    },

    clear_unread_logs(state) {
      state.logs.unread = 0;
    }
  },

  actions: {
    send_obj: function(context, obj) {
      Vue.prototype.$socket.sendObj(obj);
    },
    send_request: (context, { type, data }) => {
      let obj = { request: type };
      if (Object.keys(data).length !== 0) {
        obj["data"] = data;
      }
      context.dispatch("send_obj", obj);
    },
    connect_server: (context, val) => {
      context.commit("set_socket_connecting", true);
      Vue.prototype.$connect("ws://" + val);
    }
  },

  modules: {}
});
