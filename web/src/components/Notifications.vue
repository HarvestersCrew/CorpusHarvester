<template>
  <v-snackbar :color="color" v-model="displayed">
    <span>{{ msg }}</span>
    <v-spacer></v-spacer>
    <v-btn icon :dark="dark" @click="clear"><v-icon>mdi-close</v-icon></v-btn>
  </v-snackbar>
</template>

<script>
import { mapState } from "vuex";
export default {
  name: "Notifications",
  data() {
    return {
      local_queue: [],
      displayed: false,
      dark: true,
      color: undefined,
      msg: undefined,
      snack_available: true
    };
  },
  computed: mapState({
    notifications: "notifications"
  }),
  watch: {
    displayed: function(new_val) {
      if (new_val === false) {
        window.setTimeout(() => {
          this.local_queue.shift();
          this.snack_available = true;
        }, 200);
      }
    },
    snack_available: function(new_val) {
      if (new_val && this.local_queue.length > 0) {
        this.display();
      }
    },
    notifications: function() {
      if (this.notifications.length > 0) {
        if (this.local_queue.length > 0) this.clear();
        this.local_queue.push(this.notifications.shift());
      }
      if (this.snack_available === true) {
        this.display();
      }
    }
  },
  methods: {
    display() {
      this.snack_available = false;
      this.msg = this.local_queue[0].msg;
      this.dark = this.local_queue[0].dark;
      this.color = this.local_queue[0].color;
      this.displayed = true;
    },
    clear() {
      this.displayed = false;
    }
  }
};
</script>
