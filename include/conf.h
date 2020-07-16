/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e /  c o n f . h                                **/
/*******************************************************************/

#ifndef CONF_H
#define CONF_H

#define ALL -1
#define RESTART 1
#define NO_RESTART 0
#define FIRE 1
#define NOFIRE 0
#define RANDOM_PREC 1
#define INTERPOLATE_PREC 0
#define GRID 0
#define FPC 1
#define MNPP 2
#define MRH 3
#define MTRANSP  4
#define MRUNOFF 5
#define MDISCHARGE 6
#define MEVAP 7
#define MINTERC 8
#define MSWC1  9
#define MSWC2 10
#define FIREC 11
#define FIREF 12
#define VEGC 13
#define SOILC 14
#define LITC 15
#define FLUX_ESTAB 16
#define HARVEST 17
#define MIRRIG_WD 18
#define SDATE 19
#define PFT_NPP 20
#define PFT_HARVEST 21
#define PFT_RHARVEST 22
#define PFT_GCGP 23
#define PFT_FO3UPTAKE 24
#define PFT_TRANSP 25
#define PFT_GC 26
#define PFT_LAI 27
#define PFT_GPP 28
#define PFT_VEGC 29
#define PFT_NIND 30
#define PFT_MORT 31
#define MGPP 32
#define MSOILTEMP 33
#define MPET 34
#define MPAR 35
#define MRA 36
#define PFT_MAXPHENDAY 37
#define PFT_BIMONFPAR 38
#define FLUX_LUC 39
#define MSNOWPACK 40
#define MPFT_LAI 41
#define MPFT_GC 42
#define MPFT_CI 43
#define MPFT_TRANSP 44
#define MPFT_GPP 45

/*
  permafrost outputs
  ..flag required to write
*/

#define WTD 46
#define WETFRAC 47
#define MCH4E 48
#define CH4O 49
#define MTSOIL0 50
#define MTSOIL10 51
#define MTSOIL25 52
#define MTSOIL50 53
#define MTSOIL70 54
#define MTSOIL100 55
#define MTSOIL150 56
#define MTSOIL200 57
#define MTSOILTOP 58
#define MTHAW_DEPTH 59
#define MFWATER_10 60
#define MFWATER_20 61
#define MFWATER_30 62
#define MFWATER_50 63
#define MFWATER_70 64
#define MFWATER_100 65
#define MFWATER_150 66
#define MFWATER_200 67
#define MFICE_10 68
#define MFICE_20 69
#define MFICE_30 70
#define MFICE_50 71
#define MFICE_70 72
#define MFICE_100 73
#define MFICE_150 74
#define MFICE_200 75
#define FROZEN_DAYS 76

/*
   output for wood harvest products,
   .. and deforestation products (grid-cell level)
   .. only write if flag used
 */
#define WOODHARVEST_100YR 77
#define WOODHARVEST_10YR 78
#define WOODHARVEST_1YR 79
#define WOODHARVEST_100YR_REMAIN 80
#define WOODHARVEST_10YR_REMAIN 81
#define WOODHARVEST_1YR_REMAIN 82

#define DEFORESTPRODUCT_25YR 83
#define DEFORESTPRODUCT_2YR 84
#define DEFORESTPRODUCT_25YR_REMAIN 85
#define DEFORESTPRODUCT_2YR_REMAIN 86

/*
    output by primary/secondary stands
  ..carbon pools are stdout (veg,litter,soil)
  ..flux_luc_ is stdout
  ..following stand-specific outputs require flag to write:
      flux_estab_
      firec_
      woodharvest_
      deforestationproducts_
 */
#define FRAC_PRIMARY 87
#define FPC_PRIMARY 88
#define MNPP_PRIMARY 89
#define MGPP_PRIMARY 90
#define MRH_PRIMARY 91
#define VEGC_PRIMARY 92
#define SOILC_PRIMARY 93
#define LITC_PRIMARY 94
#define FLUX_LUC_PRIMARY 95
#define FLUX_ESTAB_PRIMARY 96
#define FIREC_PRIMARY 97
#define WOODHARVEST_100YR_PRIMARY 98
#define WOODHARVEST_10YR_PRIMARY 99
#define WOODHARVEST_1YR_PRIMARY 100
#define WOODHARVEST_100YR_REMAIN_PRIMARY 101
#define WOODHARVEST_10YR_REMAIN_PRIMARY 102
#define WOODHARVEST_1YR_REMAIN_PRIMARY 103
#define DEFORESTPRODUCT_25YR_PRIMARY 104
#define DEFORESTPRODUCT_2YR_PRIMARY 105
#define DEFORESTPRODUCT_25YR_REMAIN_PRIMARY 106
#define DEFORESTPRODUCT_2YR_REMAIN_PRIMARY 107

#define FRAC_SECFOREST 108
#define FPC_SECFOREST 109
#define MNPP_SECFOREST 110
#define MGPP_SECFOREST 111
#define MRH_SECFOREST 112
#define VEGC_SECFOREST 113
#define SOILC_SECFOREST 114
#define LITC_SECFOREST 115
#define FLUX_LUC_SECFOREST 116
#define FLUX_ESTAB_SECFOREST 117
#define FIREC_SECFOREST 118
#define WOODHARVEST_100YR_SECFOREST 119
#define WOODHARVEST_10YR_SECFOREST 120
#define WOODHARVEST_1YR_SECFOREST 121
#define WOODHARVEST_100YR_REMAIN_SECFOREST 122
#define WOODHARVEST_10YR_REMAIN_SECFOREST 123
#define WOODHARVEST_1YR_REMAIN_SECFOREST 124
#define DEFORESTPRODUCT_25YR_SECFOREST 125
#define DEFORESTPRODUCT_2YR_SECFOREST 126
#define DEFORESTPRODUCT_25YR_REMAIN_SECFOREST 127
#define DEFORESTPRODUCT_2YR_REMAIN_SECFOREST 128

/*
  daily outputs
  ..flag required to write
*/
#define DWET_FRAC 129
#define DCH4E 130
#define DGPP 131
#define DNPP 132
#define DRH 133
#define DFWATER_10 134
#define DFWATER_20 135
#define DFWATER_30 136
#define DFWATER_50 137
#define DFWATER_70 138
#define DFWATER_100 139
#define DFWATER_150 140
#define DFWATER_200 141
#define DFICE_10 142
#define DFICE_20 143
#define DFICE_30 144
#define DFICE_50 145
#define DFICE_70 146
#define DFICE_100 147
#define DFICE_150 148
#define DFICE_200 149

/*
  ageclass outputs
  ..flag required to write
*/

#define AGECLASS_PRI_FRAC 150
#define AGECLASS_PRI_FIREC 151
#define AGECLASS_PRI_FLUXESTAB 152
#define AGECLASS_PRI_VEGC 153
#define AGECLASS_PRI_MNPP 154
#define AGECLASS_PRI_MRH 155
#define AGECLASS_PRI_PFTFPC 156
#define AGECLASS_SEC_FRAC 157
#define AGECLASS_SEC_FIREC 158
#define AGECLASS_SEC_FLUXESTAB 159
#define AGECLASS_SEC_VEGC 160
#define AGECLASS_SEC_MNPP 161
#define AGECLASS_SEC_MRH 162
#define AGECLASS_SEC_PFTFPC 163

/*
    agriculture monthly pft-specific outputs 
    ..flag required to write (OUTPUT_AGRICULTURE_MPFT)
*/ 

#define MPFT_AG_NPP 164
#define MPFT_AG_GPP 165
#define MPFT_AG_LAI 166
#define MPFT_AG_TRANSP 167 /* transpiration */

/* climate variables */
#define MTAIR 168
#define MPPT 169
#define MWETDAYS 170
#define MSWDOWN 171
#define MLWDOWN 172
#define MCLD 173
#define MCO2CONC 174
#define DTAIR 175
#define DPPT 176
#define DSWDOWN 177
#define DLWDOWN 178
#define DCLD 179
#define MISOPR 180
#define MMONOPR 181
#define RANDOM_SEED 0

#endif                
