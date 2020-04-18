<template>
  <v-form>
    <p class="subtitle-2 text-left">{{ param.name }}</p>

    <v-text-field
      outlined
      dense
      clearable
      v-if="param.values.length === 0"
      :placeholder="placeholder"
      :hint="hint_text"
      persistent-hint
      :type="type"
      :value="value"
      @input="$emit('input', $event)"
    ></v-text-field>

    <v-select
      outlined
      dense
      clearable
      v-else
      :placeholder="placeholder"
      :hint="hint_text"
      persistent-hint
      :items="param.values"
      :value="value"
      @input="$emit('input', $event)"
    ></v-select>
  </v-form>
</template>

<script>
export default {
  props: {
    param: Object,
    value: [String, Number]
  },
  name: "ApiParamInput",
  computed: {
    placeholder() {
      return this.param.default_value !== null
        ? this.param.default_value
        : undefined;
    },
    hint_text() {
      return this.param.required ? "Required" : undefined;
    },
    type() {
      if (this.param.value_type === "int") return "number";
      else return "text";
    }
  },
  methods: {
    test(data) {
      console.log(data);
    }
  }
};
</script>
