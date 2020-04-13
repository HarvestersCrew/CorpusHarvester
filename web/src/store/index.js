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
    }
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
    SOCKET_ONMESSAGE(state, message) {
      state.socket.message = message;
    },

    set_socket_connecting(state, boolval) {
      state.socket.connecting = boolval;
    }
  },
  actions: {
    // sendMessage: function(context, message) {
    //   Vue.prototype.$socket.send(message);
    // }
    connect_server: (context, val) => {
      context.commit("set_socket_connecting", true);
      Vue.prototype.$connect("ws://" + val);
    }
  },
  modules: {}
});
