import { Doughnut, mixins } from "vue-chartjs";
const { reactiveProp } = mixins;

export default {
  extends: Doughnut,
  props: {
    options: {
      type: Object,
      default: () => {}
    }
  },
  mixins: [reactiveProp],
  mounted() {
    this.renderChart(this.chartData, this.options);
  }
};
