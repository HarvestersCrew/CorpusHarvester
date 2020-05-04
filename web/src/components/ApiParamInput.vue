<template>
  <v-row dense>
    <v-col v-if="builder_type !== 'web'" cols="auto">
      <v-card outlined>
        <v-list dense>
          <v-list-group v-model="op_down">
            <template v-slot:activator>
              <v-list-item-title>{{ input_op }}</v-list-item-title>
            </template>
            <v-list-item
              dense
              v-for="(val, idx) in logical_symbols"
              :key="val"
              link
              v-model="active_op[idx]"
              @click="change_op(val)"
            >
              <v-list-item-subtitle>
                {{ val }}
              </v-list-item-subtitle>
            </v-list-item>
          </v-list-group>
        </v-list>
      </v-card>
    </v-col>

    <v-col>
      <v-text-field
        :label="label"
        outlined
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
    </v-col>
  </v-row>
</template>

<script>
export default {
  props: {
    param: { type: Object, required: true },
    value: [String, Number],
    op: String,
    disabled: { type: Boolean, required: true },
    builder_type: { required: true, type: String }
  },
  name: "ApiParamInput",
  data() {
    return {
      input_op: "",
      op_down: false
    };
  },
  mounted() {
    if (this.logical_symbols.indexOf(this.op) != -1) this.input_op = this.op;
    else this.change_op(this.logical_symbols[0]);
  },
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
    },
    logical_symbols() {
      let arr = ["=", "!="];
      if (this.type === "number") {
        arr.push("<", ">", "<=", ">=");
      }
      return arr;
    },
    active_op() {
      let arr = this.logical_symbols;
      return arr.map(el => this.input_op === el);
    }
  },
  methods: {
    change_op(val) {
      this.input_op = val;
      this.op_down = false;
      this.$emit("op_change", this.input_op);
    },
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
