<script async src="//busuanzi.ibruce.info/busuanzi/2.3/busuanzi.pure.mini.js">
</script>

<div class="copyright" >
  {% set current = date(Date.now(), "YYYY") %}
  &copy; {% if theme.since and theme.since != current %} {{ theme.since }} - {% endif %}
  <span itemprop="copyrightYear">{{ current }}</span>
  <span class="with-love">
    <i class="fa fa-{{ theme.authoricon }}"></i>
  </span>
  <span class="author" itemprop="copyrightHolder">{{ config.author }}</span>


  {% if theme.post_wordcount.totalcount %}
	<div class="post-wordcount">
		{% if not theme.post_wordcount.separated_meta %}
		  <span class="post-meta-divider">|</span>
		{% endif %}
		<span class="post-meta-item-icon">
		  <i class="fa fa-area-chart"></i>
		</span>
		{% if theme.post_wordcount.item_text %}
		  <span class="post-meta-item-text">{{ __('post.totalcount') }}</span>
		{% endif %}
		<span title="{{ __('post.totalcount') }}">
		  {{ totalcount(site) }}
		</span>
	</div>
   {% endif %}
 </div>

{% if theme.copyright %}
<div class="powered-by">
  {{ __('footer.powered', '<a class="theme-link" href="https://hexo.io">Hexo</a>') }}
</div>

<div class="theme-info">
  {{ __('footer.theme') }} -
  <a class="theme-link" href="https://github.com/iissnan/hexo-theme-next">
    NexT.{{ theme.scheme }}
  </a>
</div>
{% endif %}

