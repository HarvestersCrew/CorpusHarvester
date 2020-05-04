<template>
  <v-row dense>
    <v-col v-if="builder_type !== 'web'" cols="auto">
      <v-speed-dial direction="bottom" v-model="op_down">
        <template v-slot:activator>
          <v-btn class="px-0">
            <div class="font-weight-bold">{{ input_op }}</div>
            <div class="ml-2">
              <v-icon dense>
                {{ op_down ? "mdi-menu-up" : "mdi-menu-down" }}
              </v-icon>
            </div>
          </v-btn>
        </template>
        <v-btn
          small
          rounded
          v-for="val in logical_symbols"
          :key="val"
          :color="val === input_op ? 'blue' : undefined"
          :dark="val === input_op"
          @click="change_op(val)"
        >
          {{ val }}
        </v-btn>
      </v-speed-dial>
    </v-col>

    <v-col>
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
    }
  },
  methods: {
    change_op(val) {
      this.input_op = val;
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
