import Vue from "vue";
import Vuex from "vuex";
import router from "../router";

Vue.use(Vuex);

export default new Vuex.Store({
  state: {
    home_page: "Files",
    socket: {
      url: undefined,
      isConnected: false,
      connecting: false,
      error: false
    },
    first_init_done: false,
    apis: undefined
  },
  mutations: {
    SOCKET_ONOPEN(state, event) {
      Vue.prototype.$socket = event.currentTarget;
      state.socket.url = event.currentTarget.url;
      state.socket.isConnected = true;
      state.socket.connecting = false;
      state.socket.error = false;
      router.push({ name: state.home_page });
    },
    SOCKET_ONCLOSE(state) {
      state.socket.isConnected = false;
      state.socket.connecting = false;
      state.socket.error = true;
      state.socket.url = undefined;
    },
    SOCKET_ONERROR(state, event) {
      console.error(state, event);
    },
    // default handler called for all methods
    SOCKET_ONMESSAGE() {
      // console.log(message);
    },

    set_socket_connecting(state, boolval) {
      state.socket.connecting = boolval;
    },

    set_first_init_done(state, boolval) {
      state.first_init_done = boolval;
    },

    SOCKET_get_apis(state, data) {
      // state.apis = data;
      console.log(data);
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
