<template>
  <v-card flat tile>
    <v-toolbar color="blue lighten-2">
      <v-container fluid>
        <v-row>
          <slot></slot>
        </v-row>
      </v-container>
    </v-toolbar>
  </v-card>
</template>

<script>
export default {
  name: "SelectCard"
};
</script>
