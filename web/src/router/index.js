import Vue from "vue";
import VueRouter from "vue-router";
import Gateway from "../components/Gateway.vue";

Vue.use(VueRouter);

const routes = [
  {
    path: "/",
    name: "Gateway",
    component: Gateway
  },
  {
    path: "/login",
    name: "Login",
    // route level code-splitting
    // this generates a separate chunk (about.[hash].js) for this route
    // which is lazy-loaded when the route is visited.
    component: () => import("../views/Login.vue")
  },
  {
    path: "/files",
    name: "Files",
    // route level code-splitting
    // this generates a separate chunk (about.[hash].js) for this route
    // which is lazy-loaded when the route is visited.
    component: () => import("../views/Files.vue")
  },
  {
    path: "/corpus",
    name: "Corpus",
    component: () => import("../views/Corpus.vue")
  },
  {
    path: "/settings",
    name: "Settings",
    component: () => import("../views/Settings.vue")
  },
  {
    path: "/info",
    name: "Info",
    component: () => import("../views/Info.vue")
  }
];

const router = new VueRouter({
  mode: "history",
  base: process.env.BASE_URL,
  routes
});

export default router;
