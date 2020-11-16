import Vue from 'vue'
import Router from 'vue-router'
import Now from '../components/Now.vue'

Vue.use(Router)

export default new Router({
  mode: 'history',
  routes: [
    {
      path: '/date',
      name: 'Date',
      component: Now
    }
  ]
})
