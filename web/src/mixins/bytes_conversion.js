export const bytes_conversion = {
  methods: {
    bytes_conversion_string(bytes) {
      let units = ["B", "KB", "MB", "GB", "TB"];
      let divided_size = bytes;
      let str = "";
      let i;
      for (i = 0; i < units.length - 1; ++i) {
        if (divided_size < 1000) break;
        divided_size /= 1000;
      }
      str += Math.round(divided_size * 100) / 100 + " " + units[i];
      return str;
    }
  }
};
