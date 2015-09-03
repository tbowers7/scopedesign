pro make_spot_diagrams

;; IDL routine for making spot diagrams for the ray_trace.c program.

  ;; Make an array of the off-axis angles in arcsec
  angles = [0,1,3,5,10,30,60,120,240,600,1200,3000]
  
  n = n_e(angles)
  
  set_plot,'ps'
  !p.font=0
  
  
  FOR i=0L, n-1 DO BEGIN

     
     file = string(angles[i],format="('rt_focalplane_ang',I0,'.dat')")

     fp = rascii(file)
     
     x = fp[0,*]*100.
     y = fp[1,*]*100.

     tit = string(angles[i],$
                  format="('Spot Diagram for off-axis angle = ',I0,' arcsec')")
     
     psfile = string(angles[i],format="('spot_diagram_ang',I0,'.eps')")
     device,filename=psfile,/encapsul,/isolatin1
     
     plot,x,y,/isotropic,psym=1,xtitle='Position in Focal Plane (cm)',$
          ytitle='Position in Focal Plane (cm)',title=tit,symsize=0.25
     
     device,/close_file
          
     
  ENDFOR
  
  
  !p.font=-1
  
end
