import Vue from "vue";
import Vuex from "vuex";

Vue.use(Vuex);

export default new Vuex.Store({
  state: {
    server: undefined
  },
  mutations: {
    set_server: (state, val) => {
      state.server = val;
    }
  },
  actions: {},
  modules: {}
});
