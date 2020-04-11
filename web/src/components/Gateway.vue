<template>
  <div></div>
</template>
<script>
export default {
  name: "Gateway",
  beforeCreate() {
    // Will check presence in local storage of a server
    this.$router.push("login");
  }
};
</script>
