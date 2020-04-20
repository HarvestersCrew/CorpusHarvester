<template>
  <div>
    <v-text-field
      :label="label"
      outlined
      dense
      clearable
      v-if="param.values === undefined || param.values.length === 0"
      :placeholder="param.default_value"
      :hint="hint_text"
      persistent-hint
      :value="value"
      @input="$emit('input', $event)"
      :rules="[validation]"
      :disabled="disabled"
    ></v-text-field>

    <v-select
      :label="label"
      outlined
      dense
      clearable
      v-else
      :placeholder="param.default_value"
      :hint="hint_text"
      persistent-hint
      :items="param.values"
      :value="value"
      @input="$emit('input', $event)"
      :disabled="disabled"
    ></v-select>
  </div>
</template>

<script>
export default {
  props: {
    param: { type: Object, required: true },
    value: [String, Number],
    disabled: { type: Boolean, required: true }
  },
  name: "ApiParamInput",
  computed: {
    hint_text() {
      let hint = this.param.description ? this.param.description : undefined;
      return hint;
    },
    type() {
      if (this.param.value_type === "int") return "number";
      else return "text";
    },
    label() {
      return this.param.required ? this.param.name + "*" : this.param.name;
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
