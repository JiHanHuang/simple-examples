<template>
 <div>
  <div>
    <h1>date : {{date}}</h1>
    <button v-on:click="update()">刷新</button>
  </div>
 </div>
</template>
<script>
import axios from 'axios'
export default {
  data () {
    return {
      date: null
    }
  },
  methods: {
    update () {
      axios
        .get('/v1/date')
        .then(response => (this.date = response.data.date))
        .catch(function (error) { // 请求失败处理
          console.log(error)
        })
    }
  },
  mounted () {
    axios
      .get('/v1/date')
      .then(response => (this.date = response.data.date))
      .catch(function (error) { // 请求失败处理
        console.log(error)
      })
  }
}
</script>
