<template>
  <div>
    <p class="subtitle-2 text-left">{{ param.name }}</p>

    <v-text-field
      outlined
      dense
      clearable
      v-if="param.values.length === 0"
      :placeholder="placeholder"
      :hint="hint_text"
      persistent-hint
      :value="value"
      @input="$emit('input', $event)"
      :rules="[validation]"
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
  </div>
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
        : "No default value";
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
    validation(data) {
      if (!data) {
        if (this.param.required && this.param.default_value === null)
          return "Parameter must not be empty";
        else return true;
      }
      if (this.param.value_type === "int") {
        if (!data.match(/^\d+$/)) return "Parameter must be an int";
        else return true;
      }
      return true;
    }
  }
};
</script>
