; —ÔÂˆË‡ÎËÁËÓ‚‡ÌÌ‡ˇ Ú‡·ÎËˆ‡ ÍÓ˝ÙÙËˆËÂÌÚÓ‚ ÔÓ‚ÂıÌÓÒÚÂÈ
; ı‡‡ÍÚÂËÁÛ˛˘‡ˇ ÏÓÏÂÌÚ ¿ƒ ‚ ÙÛÌÍˆËË Ì‡ÔˇÊÂÌËˇ, ÚÓÍ‡
; Ë Û„Î‡ Ì‡„ÛÁÍË.

; œÓ‰‰ÂÊË‚‡ÂÚÒˇ ÒÓ‚ÏÂÒÚÌÓÂ ËÒÔÓÎ¸ÁÓ‚‡ÌËÂ Ò Ù‡ÈÎÓÏ "torque.c" .
 ;---------------------------------------------------------------------------
 ;   Ã‡ÒÒË‚		|ÕÓÏ. ÏÓÏÂÌÚ| iÂ‰ | “ËÔ   |  “ËÔ	  |ÃÓ˘ÌÓÒÚ¸|   —ËÌı.   |
 ;  ÍÓÌÒÚ‡ÌÚ	|  ÔË‚Ó‰‡  |	   |Á‡‰‚ËÊ.|  ¿ƒ	  |		   |  ÒÍÓÓÒÚ¸  |
 ;---------------------------------------------------------------------------
 .def _drive1   ;   100 Nm,  i=52.4,  A-25, ¿»Ã-¿63¬4,  0.37Í¬Ú, 1500 Ó·/ÏËÌ|
 .def _drive2	;   100 Nm,  i=52.4,  A-50, ¿»Ã-¿63¿2,  0.37Í¬Ú, 3000 Ó·/ÏËÌ|
 .def _drive3	;   400 Nm,  i=73.7,  ¡-20, ¿»Ã-¿80¬4,  1.5Í¬Ú,  1500 Ó·/ÏËÌ|
 .def _drive4	;   400 Nm,  i=73.7,  ¡-40, ¿»Ã-¿80¬2,  1.5Í¬Ú,  3000 Ó·/ÏËÌ|
 .def _drive5	;  1000 Nm,  i=73.7,  ¬-20, ¿»Ã-¿80¬4,  1.5Í¬Ú,  1500 Ó·/ÏËÌ|
 .def _drive6	;   800 Nm,  i=73.7,  ¬-40, ¿»Ã-¿80¬2,  1.5Í¬Ú,  3000 Ó·/ÏËÌ|
 .def _drive7	;  4000 Nm,  i=167.2, √-9,  ¿»Ã-¿100S4, 2.5Í¬Ú,  1500 Ó·/ÏËÌ|
 .def _drive8	;  4000 Nm,  i=167.2, √-18, ¿»Ã-¿100S2, 4.0Í¬Ú,  3000 Ó·/ÏËÌ|
 .def _drive9	; 10000 Nm,  i=252.2, ƒ-6,  ¿»Ã-¿100L4, 4.0Í¬Ú,  1500 Ó·/ÏËÌ|
 .def _drive10	; 10000 Nm,  i=270.2, ƒ-12, ¿»Ã-¿100L2, 5.5Í¬Ú,  3000 Ó·/ÏËÌ|
 .def _drive11	;	 ?														|
 .def _drive12	;	 ?														|
 .def _drive13	;   400 Nm,  i=73.7,  ¡-20, ¿»Ã-¿80S4,  0.55Í¬Ú, 1500 Ó·/ÏËÌ|
 .def _drive14	;   400 Nm,  i=73.7,  ¡-40, ¿»Ã-¿80L2,  1.1Í¬Ú,  3000 Ó·/ÏËÌ|
 .def _drive15	;	 ?														|
 .def _drive16	;	 ?														|
 .def _drive17	;	 ?														|
 .def _drive18	;	 ?														|
 .def _drive19	;	 ?														|
 .def _drive20	;	 ?														|
;----------------------------------------------------------------------------

_drive11	;   ?	
_drive1		;	100 Nm,  i=52.4,  A-25, ¿»Ã-¿63¬4,  0.37Í¬Ú, 1500 Ó·/ÏËÌ
 .word 7504		;k10
 .word 9327		;k11
 .word 12434	;k12
 .word 18022	;k13
 .word 28963	;k14
 .word 2376		;k20
 .word 2294		;k21
 .word 2990		;k22
 .word 4833		;k23
 .word 7209		;k24
 .word 3277		;im0
 .word 4096		;im1
 .word 5243		;im2
 .word 6144		;im3
 .word 8192		;im4
 .word -3277	;fi10
 .word -6554	;fi11
 .word -9830	;fi12
 .word -12288	;fi13
 .word -20480	;fi14
 .word 5161		;fi20
 .word 9994		;fi21
 .word 14909	;fi22
 .word 19907	;fi23
 .word 31539	;fi24
 .word 655		;curr_hard
 .word 8197		;curr_fine
 .word 164		;fi_hard
 .word 8184		;fi_fine
 .word 126		;volt_hard
 .word 8194		;volt_fine
 .word 82		;torq_har
 .word 8184		;torq_fine
 .word -29082	;k0
 .word -16604	;k1
 .word -10576	;k2
 .word -7906	;k3
 .word -6275	;k4
 .word 18022	;k5
 .word 18022	;k6
 .word 17613	;k7
 .word 17613	;k8
 .word 18022	;k9
 .word 82		;torq_ref_hard
 .word 8184		;torq_ref_fine
 .word 164		;ugol_hard
 .word 8184		;ugol_fine
 .word 90		;cycle_max
 .word 14746	;120B Q13
 .word 19046	;155B Q13
 .word 23347	;190B Q13
 .word 27648	;225B Q13
 .word 15604	;1/35B Q13
 
_drive12	;   ?
_drive2 ;	100 Nm,  i=52.4,  A-50, ¿»Ã-¿63¿2,  0.37Í¬Ú, 3000 Ó·/ÏËÌ
 .word 6831		;k10
 .word 12216	;k11
 .word 12898	;k12
 .word 17541	;k13
 .word 22829	;k14
 .word 2949		;k20
 .word 4424		;k21
 .word 2621		;k22
 .word 4096		;k23
 .word 4997		;k24
 .word 5734		;im0
 .word 7373		;im1
 .word 7373		;im2
 .word 7373		;im3
 .word 7373		;im4
 .word -4915	;fi10
 .word -10240	;fi11
 .word -11469	;fi12
 .word -14746	;fi13
 .word -19661	;fi14
 .word 5734		;fi20
 .word 10977	;fi21
 .word 14336	;fi22
 .word 19087	;fi23
 .word 26624	;fi24
 .word 655		;curr_hard
 .word 8197		;curr_fine
 .word 164		;fi_hard
 .word 8184		;fi_fine
 .word 126		;volt_hard
 .word 8194		;volt_fine
 .word 164		;torq_har
 .word 8184		;torq_fine
 .word -9830	;k0
 .word -6281	;k1
 .word -3277	;k2
 .word -2587	;k3
 .word -2137	;k4
 .word 18432	;k5
 .word 18186	;k6
 .word 17531	;k7
 .word 17531	;k8
 .word 17859	;k9
 .word 164		;torq_ref_hard
 .word 8184		;torq_ref_fine
 .word 164		;ugol_hard
 .word 8184		;ugol_fine
 .word 90		;cycle_max
 .word 14746	;120B Q13
 .word 19046	;155B Q13
 .word 23347	;190B Q13
 .word 27648	;225B Q13
 .word 15604	;1/35B Q13

_drive3 	;	400 Nm,  i=73.7,  ¡-20, ¿»Ã-¿80¬4,  1.5Í¬Ú,  1500 Ó·/ÏËÌ
 .word 7231		;k10
 .word 10345	;k11
 .word 13195	;k12
 .word 17058	;k13
 .word 32064	;k14
 .word 4588		;k20
 .word 5898		;k21
 .word 7127		;k22
 .word 10035	;k23
 .word 16753	;k24
 .word 7127		;im0
 .word 9421		;im1
 .word 8520		;im2
 .word 11878	;im3
 .word 17347	;im4
 .word -2867	;fi10
 .word -4096	;fi11
 .word -4096	;fi12
 .word -8192	;fi13
 .word -9011	;fi14
 .word 8438		;fi20
 .word 12452	;fi21
 .word 14131	;fi22
 .word 27443	;fi23
 .word 32686	;fi24
 .word 328		;curr_hard
 .word 8184		;curr_fine
 .word 364		;fi_hard
 .word 8194		;fi_fine
 .word 126		;volt_hard
 .word 8194		;volt_fine
 .word 22		;torq_har
 .word 8134		;torq_fine
 .word -8798	;k0
 .word -5795	;k1
 .word -4448	;k2
 .word -3575	;k3
 .word -3466	;k4
 .word 28399	;k5
 .word 29218	;k6
 .word 29628	;k7
 .word 30119	;k8
 .word 30857	;k9
 .word 37		;torq_ref_hard
 .word 8244		;torq_ref_fine
 .word 273		;ugol_hard
 .word 8194		;ugol_fine
 .word 90		;cycle_max
 .word 14746	;120B Q13
 .word 19046	;155B Q13
 .word 23347	;190B Q13
 .word 27648	;225B Q13
 .word 15604	;1/35B Q13

_drive15	;   ?
_drive5 	;  1000 Nm,  i=73.7,  ¬-20, ¿»Ã-¿80¬4,  1.5Í¬Ú,  1500 Ó·/ÏËÌ
 .word 7771		;k10
 .word 9479		;k11
 .word 12771	;k12
 .word 16029	;k13
 .word 18572	;k14
 .word 4915		;k20
 .word 4588		;k21
 .word 5980		;k22
 .word 8847		;k23
 .word 14254	;k24
 .word 7782		;im0
 .word 6308		;im1
 .word 9011		;im2
 .word 10568	;im3
 .word 18022	;im4
 .word -2048	;fi10
 .word -2212	;fi11
 .word -4096	;fi12
 .word -7373	;fi13
 .word -8602	;fi14
 .word 7537		;fi20
 .word 8929		;fi21
 .word 14828	;fi22
 .word 25805	;fi23
 .word 31130	;fi24
 .word 328		;curr_hard
 .word 8184		;curr_fine
 .word 364		;fi_hard
 .word 8194		;fi_fine
 .word 126		;volt_hard
 .word 8194		;volt_fine
 .word 22		;torq_har
 .word 8134		;torq_fine
 .word -13975	;k0
 .word -7951	;k1
 .word -5506	;k2
 .word -3847	;k3
 .word -3554	;k4
 .word 28809	;k5
 .word 27989	;k6
 .word 28112	;k7
 .word 28399	;k8
 .word 29218	;k9
 .word 22		;torq_ref_hard
 .word 8134		;torq_ref_fine
 .word 273		;ugol_hard
 .word 8194		;ugol_fine
 .word 90		;cycle_max
 .word 14746	;120B Q13
 .word 19046	;155B Q13
 .word 23347	;190B Q13
 .word 27648	;225B Q13
 .word 15604	;1/35B Q13

_drive4 	;   400 Nm,  i=73.7,  ¡-50, ¿»Ã-¿80¬2,  1.5Í¬Ú,  3000 Ó·/ÏËÌ
 .word 5		;1.  ÃŒÃ≈Õ“ “Œ  0_0
 .word 45		;2.  ÃŒÃ≈Õ“ “Œ  0_1
 .word 66		;3.  ÃŒÃ≈Õ“ “Œ  0_2
 .word 100		;4.  ÃŒÃ≈Õ“ “Œ  0_3
 .word 120		;5.  ÃŒÃ≈Õ“ “Œ  0_4
 .word 1		;6.  ÃŒÃ≈Õ“ “Œ  1_0
 .word 45		;7.  ÃŒÃ≈Õ“ “Œ  1_1
 .word 75		;8.  ÃŒÃ≈Õ“ “Œ  1_2
 .word 100		;9.  ÃŒÃ≈Õ“ “Œ  1_3
 .word 110		;10. ÃŒÃ≈Õ“ “Œ  1_4
 .word 15		;11. ÃŒÃ≈Õ“ “Œ  2_0
 .word 62		;12. ÃŒÃ≈Õ“ “Œ  2_1
 .word 87		;13. ÃŒÃ≈Õ“ “Œ  2_2
 .word 110		;14. ÃŒÃ≈Õ“ “Œ  2_3
 .word 125		;15. ÃŒÃ≈Õ“ “Œ  2_4
 .word 70		;16. ÃŒÃ≈Õ“ “Œ  3_0
 .word 80		;17. ÃŒÃ≈Õ“ “Œ  3_1
 .word 87		;18. ÃŒÃ≈Õ“ “Œ  3_2
 .word 125		;19. ÃŒÃ≈Õ“ “Œ  3_3
 .word 135		;20. ÃŒÃ≈Õ“ “Œ  3_4
 .word 40		;21. ÃŒÃ≈Õ“ ”√ŒÀ 0_0
 .word 37		;22. ÃŒÃ≈Õ“ ”√ŒÀ 0_1
 .word 10		;23. ÃŒÃ≈Õ“ ”√ŒÀ 0_2
 .word 5		;24. ÃŒÃ≈Õ“ ”√ŒÀ 0_3
 .word 1		;25. ÃŒÃ≈Õ“ ”√ŒÀ 0_4
 .word 65		;26. ÃŒÃ≈Õ“ ”√ŒÀ 1_0
 .word 45		;27. ÃŒÃ≈Õ“ ”√ŒÀ 1_1
 .word 20		;28. ÃŒÃ≈Õ“ ”√ŒÀ 1_2
 .word 11		;29. ÃŒÃ≈Õ“ ”√ŒÀ 1_3
 .word 1		;30. ÃŒÃ≈Õ“ ”√ŒÀ 1_4
 .word 85		;31. ÃŒÃ≈Õ“ ”√ŒÀ 2_0
 .word 43		;32. ÃŒÃ≈Õ“ ”√ŒÀ 2_1
 .word 25		;33. ÃŒÃ≈Õ“ ”√ŒÀ 2_2
 .word 12		;34. ÃŒÃ≈Õ“ ”√ŒÀ 2_3
 .word 5		;35. ÃŒÃ≈Õ“ ”√ŒÀ 2_4
 .word 95		;36. ÃŒÃ≈Õ“ ”√ŒÀ 3_0
 .word 65		;37. ÃŒÃ≈Õ“ ”√ŒÀ 3_1
 .word 40		;38. ÃŒÃ≈Õ“ ”√ŒÀ 3_2
 .word 24		;39. ÃŒÃ≈Õ“ ”√ŒÀ 3_3
 .word 1		;40. ÃŒÃ≈Õ“ ”√ŒÀ 3_4
 .word 80		;41. ”√ŒÀ —»‘” 0_0 
 .word 75		;42. ”√ŒÀ —»‘” 0_1 
 .word 73		;43. ”√ŒÀ —»‘” 0_2
 .word 57		;44. ”√ŒÀ —»‘” 0_3
 .word 92		;45. ”√ŒÀ —»‘” 0_4
 .word 92		;46. ”√ŒÀ —»‘” 1_0
 .word 87		;47. ”√ŒÀ —»‘” 1_1
 .word 80		;48. ”√ŒÀ —»‘” 1_2
 .word 67		;49. ”√ŒÀ —»‘” 1_3
 .word 54		;50. ”√ŒÀ —»‘” 1_4
 .word 95		;51. ”√ŒÀ —»‘” 2_0
 .word 92		;52. ”√ŒÀ —»‘” 2_1
 .word 83		;53. ”√ŒÀ —»‘” 2_2
 .word 75		;54. ”√ŒÀ —»‘” 2_3
 .word 65		;55. ”√ŒÀ —»‘” 2_4
 .word 98		;56. ”√ŒÀ —»‘” 3_0
 .word 94		;57. ”√ŒÀ —»‘” 3_1
 .word 90		;58. ”√ŒÀ —»‘” 3_2
 .word 81		;59. ”√ŒÀ —»‘” 3_3
 .word 70		;60. ”√ŒÀ —»‘” 3_4   
 
_drive16	;   ?
_drive6 	;   800 Nm,  i=73.7,  ¬-40, ¿»Ã-¿80¬2,  1.5Í¬Ú,  3000 Ó·/ÏËÌ
 .word 5387		;k10
 .word 11660	;k11
 .word 14038	;k12
 .word 19298	;k13
 .word 26092	;k14
 .word 1939		;k20
 .word 4178		;k21
 .word 3659		;k22
 .word 5571		;k23
 .word 8137		;k24
 .word 2703		;im0
 .word 6554		;im1
 .word 7373		;im2
 .word 8437		;im3
 .word 9421		;im4
 .word -1229	;fi10
 .word -7373	;fi11
 .word -6144	;fi12
 .word -5734	;fi13
 .word -9830	;fi14
 .word 4284		;fi20
 .word 14746	;fi21
 .word 16056	;fi22
 .word 17285	;fi23
 .word 31130	;fi24
 .word 218		;curr_hard
 .word 8209		;curr_fine
 .word 328		;fi_hard
 .word 8184		;fi_fine
 .word 126		;volt_hard
 .word 8194		;volt_fine
 .word 37		;torq_har
 .word 8244		;torq_fine
 .word -16384	;k0
 .word -10587	;k1
 .word -7873	;k2
 .word -5831	;k3
 .word -4396	;k4
 .word 28399	;k5
 .word 28809	;k6
 .word 29628	;k7
 .word 29628	;k8
 .word 29751	;k9
 .word 37		;torq_ref_hard
 .word 8244		;torq_ref_fine
 .word 273		;ugol_hard
 .word 8194		;ugol_fine
 .word 90		;cycle_max
 .word 14746	;120B Q13
 .word 19046	;155B Q13
 .word 23347	;190B Q13
 .word 27648	;225B Q13
 .word 15604	;1/35B Q13

_drive17	;   ?
_drive7 	;  4000 Nm,  i=167.2, √-9,  ¿»Ã-¿100S4, 2.5Í¬Ú,  1500 Ó·/ÏËÌ
 .word 6234		;k10
 .word 8446		;k11
 .word 11078	;k12
 .word 14957	;k13
 .word 19309	;k14
 .word 4589		;k20
 .word 4178		;k21
 .word 4833		;k22
 .word 7782		;k23
 .word 13599	;k24
 .word 7373		;im0
 .word 8192		;im1
 .word 9011		;im2
 .word 9830		;im3
 .word 16056	;im4
 .word -1638	;fi10
 .word -2867	;fi11
 .word -4096	;fi12
 .word -5734	;fi13
 .word -9830	;fi14
 .word 5489		;fi20
 .word 9011		;fi21
 .word 13517	;fi22
 .word 18432	;fi23
 .word 31949	;fi24
 .word 164		;curr_hard
 .word 8184		;curr_fine
 .word 328		;fi_hard
 .word 8184		;fi_fine
 .word 126		;volt_hard
 .word 8194		;volt_fine
 .word 4		;torq_har
 .word 8389		;torq_fine
 .word -11469	;k0
 .word -8602	;k1
 .word -5188	;k2
 .word -3917	;k3
 .word -3679	;k4
 .word 26761	;k5
 .word 30037	;k6
 .word 28645	;k7
 .word 28645	;k8
 .word 30037	;k9
 .word 5		;torq_ref_hard
 .word 8948		;torq_ref_fine
 .word 273		;ugol_hard
 .word 8194		;ugol_fine
 .word 90		;cycle_max
 .word 14746	;120B Q13
 .word 19046	;155B Q13
 .word 23347	;190B Q13
 .word 27648	;225B Q13
 .word 15604	;1/35B Q13
 
_drive18	;   ?
_drive8 	;  4000 Nm,  i=167.2, √-18, ¿»Ã-¿100S2, 4.0Í¬Ú,  3000 Ó·/ÏËÌ
 .word 4980		;k10
 .word 5915		;k11
 .word 9285		;k12
 .word 11881	;k13
 .word 15663	;k14
 .word 2621		;k20
 .word 2621		;k21
 .word 3359		;k22
 .word 5980		;k23
 .word 10240	;k24
 .word 3686		;im0
 .word 4915		;im1
 .word 6554		;im2
 .word 9011		;im3
 .word 13926	;im4
 .word -2007	;fi10
 .word -1909	;fi11
 .word -4096	;fi12
 .word -5980	;fi13
 .word -9830	;fi14
 .word 5136		;fi20
 .word 5202		;fi21
 .word 9830		;fi22
 .word 16384	;fi23
 .word 28672	;fi24
 .word 82		;curr_hard
 .word 8184		;curr_fine
 .word 328		;fi_hard
 .word 8184		;fi_fine
 .word 126		;volt_hard
 .word 8194		;volt_fine
 .word 4		;torq_har
 .word 8389		;torq_fine
 .word -29082	;k0
 .word -16271	;k1
 .word -9436	;k2
 .word -7368	;k3
 .word -5639	;k4
 .word 30447	;k5
 .word 29628	;k6
 .word 28399	;k7
 .word 29464	;k8
 .word 30037	;k9
 .word 5		;torq_ref_hard
 .word 8948		;torq_ref_fine
 .word 273		;ugol_hard
 .word 8194		;ugol_fine
 .word 90		;cycle_max
 .word 14746	;120B Q13
 .word 19046	;155B Q13
 .word 23347	;190B Q13
 .word 27648	;225B Q13
 .word 15604	;1/35B Q13
  
_drive19	;   ?
_drive9		;	10000 Nm,  i=252.2, ƒ-6,  ¿»Ã-¿100L4, 4.0Í¬Ú,  1500 Ó·/ÏËÌ
 .word 6166		;k0 => k10
 .word 8890		;k1 => k11
 .word 11474	;k2 => k12
 .word 14773	;k3 => k13
 .word 22996	;k4 => k14
 .word 2687		;k5 => k20
 .word 3277		;k6 => k21
 .word 4096		;k7 => k22
 .word 6144		;k8 => k23
 .word 10486	;k9 => k24
 .word 4096		;k10 => im0
 .word 5243		;k11 => im1
 .word 5898		;k12 => im2
 .word 7618		;k13 => im3
 .word 11715	;k14 => im4
 .word -1475	;k15 => fi10
 .word -2294	;k16 => fi11
 .word -3359	;k17 => fi12
 .word -5980	;k18 => fi13
 .word -9830	;k19 => fi14
 .word 4342		;k20 => fi20
 .word 7127		;k21 => fi21
 .word 10650	;k22 => fi22
 .word 18842	;k23 => fi23
 .word 31130	;k24 => fi24
 .word 82		;k25 => curr_hard
 .word 8184		;k26 => curr_fine
 .word 328		;k27 => fi_hard
 .word 8184		;k28 => fi_fine
 .word 126		;k29 => volt_hard
 .word 8194		;k30 => volt_fine
 .word 2		;k31 => torq_hard
 .word 10001	;k32 => 1/torq_ref_fine
 .word -22456	;k33 => k0
 .word -14532	;k0 => k1
 .word -8704	;k0 => k2
 .word -6476	;k0 => k3
 .word -5434	;k0 => k4
 .word 28562	;k0 => k5
 .word 28808	;k0 => k6
 .word 28071	;k0 => k7
 .word 28235	;k0 => k8
 .word 28726	;k0 => k9
 .word 2		;k0 => torq_ref_hard
 .word 6710		;k0 => torq_ref_fine
 .word 273		;k0 => ugol_hard
 .word 8194		;k0 => ugol_fine
 .word 89		;k0 => cycle_max
 .word 15121	;k0 => 120B Q13
 .word 19530	;k0 => 155B Q13
 .word 23942	;k0 => 190B Q13
 .word 28352	;k0 => 225B Q13
 .word 15218	;k0 => 1/35B Q13

_drive10	;	10000 Nm,  i=270.2, ƒ-12, ¿»Ã-¿100L2, 5.5Í¬Ú,  3000 Ó·/ÏËÌ
 .word 6201		;k10
 .word 7823		;k11
 .word 11670	;k12
 .word 17373	;k13
 .word 18638	;k14
 .word 5046		;k20
 .word 4014		;k21
 .word 5325		;k22
 .word 10650	;k23
 .word 14991	;k24
 .word 7946		;im0
 .word 8192		;im1
 .word 9011		;im2
 .word 13926	;im3
 .word 19661	;im4
 .word -4260	;fi10
 .word -2949	;fi11
 .word -4096	;fi12
 .word -5734	;fi13
 .word -9503	;fi14
 .word 11911	;fi20
 .word 9503		;fi21
 .word 12616	;fi22
 .word 18842	;fi23
 .word 32604	;fi24
 .word 66		;curr_hard
 .word 8134		;curr_fine
 .word 328		;fi_hard
 .word 8184		;fi_fine
 .word 126		;volt_hard
 .word 8194		;volt_fine
 .word 3		;torq_har
 .word 8948		;torq_fine
 .word -27725	;k0
 .word -16614	;k1
 .word -14124	;k2
 .word -10132	;k3
 .word -7511	;k4
 .word 30037	;k5
 .word 29628	;k6
 .word 29628	;k7
 .word 30201	;k8
 .word 30037	;k9
 .word 2		;torq_ref_hard
 .word 8948		;torq_ref_fine
 .word 273		;ugol_hard
 .word 8194		;ugol_fine
 .word 90		;cycle_max
 .word 14746	;120B Q13
 .word 19046	;155B Q13
 .word 23347	;190B Q13
 .word 27648	;225B Q13
 .word 15604	;1/35B Q13

_drive13	;   400 Nm,  i=73.7,  ¡-20, ¿»Ã-¿80S4,  0.55Í¬Ú, 1500 Ó·/ÏËÌ
 .word 3135		;k10
 .word 4550		;k11
 .word 6763		;k12
 .word 8692		;k13
 .word 12626	;k14
 .word 3850		;k20
 .word 4424		;k21
 .word 6714		;k22
 .word 8765		;k23
 .word 13517	;k24
 .word 5325		;im0
 .word 7782		;im1
 .word 8765		;im2
 .word 10240	;im3
 .word 15729	;im4
 .word -2007	;fi10
 .word -3400	;fi11
 .word -4506	;fi12
 .word -7168	;fi13
 .word -10650	;fi14
 .word 3195		;fi20
 .word 5407		;fi21
 .word 7578		;fi22
 .word 11469	;fi23
 .word 17695	;fi24
 .word 655		;curr_hard
 .word 8197		;curr_fine
 .word 164		;fi_hard
 .word 8184		;fi_fine
 .word 126		;volt_hard
 .word 8194		;volt_fine
 .word 20		;torq_har
 .word 8389		;torq_fine
 .word -14217	;k0
 .word -8228	;k1
 .word -5514	;k2
 .word -4136	;k3
 .word -3491	;k4
 .word 30037	;k5
 .word 29464	;k6
 .word 29628	;k7
 .word 30037	;k8
 .word 30037	;k9
 .word 55		;torq_ref_hard
 .word 8134		;torq_ref_fine
 .word 273		;ugol_hard
 .word 8194		;ugol_fine
 .word 90		;cycle_max
 .word 14746	;120B Q13
 .word 19046	;155B Q13
 .word 23347	;190B Q13
 .word 27648	;225B Q13
 .word 15604	;1/35B Q13

_drive14	;	400 Nm,  i=73.7,  ¡-40, ¿»Ã-¿80L2,  1.1Í¬Ú,  3000 Ó·/ÏËÌ
 .word 4786		;k10
 .word 6039		;k11
 .word 10166	;k12
 .word 13984	;k13
 .word 15366	;k14
 .word 4506		;k20
 .word 2867		;k21
 .word 5734		;k22
 .word 6963		;k23
 .word 7127		;k24
 .word 1925		;im0
 .word 6554		;im1
 .word 7782		;im2
 .word 9830		;im3
 .word 9830		;im4
 .word -1966	;fi10
 .word -2130	;fi11
 .word -2867	;fi12
 .word -4506	;fi13
 .word -6144	;fi14
 .word 4874		;fi20
 .word 6144		;fi21
 .word 9011		;fi22
 .word 13599	;fi23
 .word 18842	;fi24
 .word 410		;curr_hard
 .word 8184		;curr_fine
 .word 328		;fi_hard
 .word 8184		;fi_fine
 .word 126		;volt_hard
 .word 8194		;volt_fine
 .word 41		;torq_har
 .word 8184		;torq_fine
 .word -19251	;k0
 .word -11296	;k1
 .word -8304	;k2
 .word -5191	;k3
 .word -4849	;k4
 .word 30037	;k5
 .word 29628	;k6
 .word 29628	;k7
 .word 28809	;k8
 .word 30037	;k9
 .word 55		;torq_ref_hard
 .word 8134		;torq_ref_fine
 .word 273		;ugol_hard
 .word 8194		;ugol_fine
 .word 90		;cycle_max
 .word 14746	;120B Q13
 .word 19046	;155B Q13
 .word 23347	;190B Q13
 .word 27648	;225B Q13
 .word 15604	;1/35B Q13
 
_drive20	;   	10000 Nm,  i=270.2, ƒ-12, ƒ¿“› , 7.5Í¬Ú,  3000 Ó·/ÏËÌ
 .word 5046		;k10
 .word 8163		;k11
 .word 12398	;k12
 .word 17111	;k13
 .word 21016	;k14
 .word 5046		;k20
 .word 4014		;k21
 .word 5325		;k22
 .word 5865		;k23
 .word 5898		;k24
 .word 7946		;im0
 .word 8192		;im1
 .word 9830		;im2
 .word 7372		;im3 ËÁÏÂÌÂÌÓ œ¿ 
 .word 9175		;im4 ËÁÏÂÌÂÌÓ œ¿ 
 .word -3113	;fi10
 .word -2130	;fi11
 .word -3277	;fi12
 .word -7045	;fi13
 .word -7373	;fi14
 .word 8684		;fi20
 .word 6881		;fi21
 .word 10650	;fi22
 .word 20480	;fi23
 .word 24576	;fi24
 .word 66		;curr_hard
 .word 8134		;curr_fine
 .word 328		;fi_hard
 .word 8184		;fi_fine
 .word 126		;volt_hard
 .word 8194		;volt_fine
 .word 2		;torq_har
 .word 9587		;torq_fine
 .word -22392	;k0
 .word -13919	;k1
 .word -9206	;k2
 .word -4570	;k3
 .word -4153	;k4
 .word 30037	;k5
 .word 29628	;k6
 .word 29628	;k7
 .word 29218	;k8
 .word 30037	;k9
 .word 2		;torq_ref_hard
 .word 8948		;torq_ref_fine
 .word 273		;ugol_hard
 .word 8194		;ugol_fine
 .word 90		;cycle_max
 .word 14746	;120B Q13
 .word 19046	;155B Q13
 .word 23347	;190B Q13
 .word 27648	;225B Q13
 .word 15604	;1/35B Q13 