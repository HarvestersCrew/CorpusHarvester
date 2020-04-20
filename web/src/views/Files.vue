<template>
  <Bar>
    <Builder builder_type="db"></Builder>
  </Bar>
</template>

<script>
import Bar from "@/components/Bar.vue";
import Builder from "@/components/Builder.vue";
export default {
  name: "Files",
  components: { Bar, Builder },
  data() {
    return {
      files: [
        {
          file_name: "A.txt",
          file_type: "Text",
          file_source: "Twitter",
          file_date: new Date().toISOString().substring(0, 10)
        },
        {
          file_name: "B.txt",
          file_type: "Text",
          file_source: "Twitter",
          file_date: new Date().toISOString().substring(0, 10)
        },
        {
          file_name: "C.txt",
          file_type: "Text",
          file_source: "Twitter",
          file_date: new Date().toISOString().substring(0, 10)
        },
        {
          file_name: "D.png",
          file_type: "Image",
          file_source: "Twitter",
          file_date: new Date().toISOString().substring(0, 10)
        },
        {
          file_name: "E.png",
          file_type: "Image",
          file_source: "Twitter",
          file_date: new Date().toISOString().substring(0, 10)
        },
        {
          file_name: "F.txt",
          file_type: "Text",
          file_source: "TMDB",
          file_date: new Date().toISOString().substring(0, 10)
        }
      ],
      filters: {
        date: undefined,
        selectedSources: [],
        selectedTypes: []
      }
    };
  },
  methods: {
    capitalize_first_letter: string =>
      string.charAt(0).toUpperCase() + string.slice(1)
  },
  computed: {
    capitalized_types: function() {
      if (this.$store.state.apis !== undefined)
        return this.$store.getters.api_types.map(type =>
          this.capitalize_first_letter(type)
        );
      else return [];
    }
  }
};
</script>
