pro make_spectrum

;; IDL routine for making spectra for the ray_trace.c program.

  ;; Make an array of the wavelengths in angstroms
  lambda = [1200,1300,1400,1500,1600,1700,1800,1900,2000]
  
  n = n_e(lambda)
  
  set_plot,'x'
;  window,0

  ;;!p.font=0
  set_plot,'ps'
  !p.font=0
  device,filename='detector_plane.eps',/encapsul,/isolatin1
  
  
  FOR i=0L, n-1 DO BEGIN
     
     
     file = string(lambda[i],format="('detector_plane_lambda',I0,'.00.dat')")

     fp = rascii(file)
     
     x = fp[0,*]*100.         ;; Detector position in cm
     y = fp[1,*]*100.         ;; Detector position in cm

     print,string(lambda[i],mean(x),format="('For wavelength ',I0,'A, the mean x-position is ',F6.2,'cm')")


     tit = 'Cylindrical Detector'
     
     ;;psfile = string(angles[i],format="('spot_diagram_ang',I0,'.eps')")
     
     IF (i EQ 0) THEN BEGIN
        plot,x,y,psym=1,xtitle='Position along detector (cm)',$
             ytitle='Position along detector (cm)',symsize=0.25,$
             xrange=[-35,35],/xstyle,yrange=[-0.1,0.1],title=tit
     ENDIF ELSE BEGIN
        oplot,x,y,psym=1,symsize=0.25
     ENDELSE

     
     
  ENDFOR
  device,/close_file
  set_plot,'x'

;  window,1
  
  file1 = 'detector_plane_lambda2000.00.dat'
  file2 = 'detector_plane_lambda2000.10.dat'

  fp1 = rascii(file1)
  fp2 = rascii(file2)
  
  x1 = fp1[0,*]*100.         ;; Detector position in cm
  y1 = fp1[1,*]*100.         ;; Detector position in cm

  x2 = fp2[0,*]*100.         ;; Detector position in cm
  y2 = fp2[1,*]*100.         ;; Detector position in cm
  
  set_plot,'ps'
  !p.font=0

  device,filename='resolving_2000A.eps',/encapsul,/isolatin1
  plot,x1,y1,psym=1,symsize=0.25,xrange=[28.8,29.0],/xstyle,$
       xtitle='Position along Cylindrical Detector (cm)',$
       ytitle='Position along Cylindrical Detector (cm)',$
       title='Spectral Separation: 1/20,000 @ 2000A' 
  oplot,x2,y2,psym=1,symsize=0.25
  device,/close_file

  set_plot,'x'



  
  !p.font=-1
  
end
