;;;; "stdlib.scm", Scheme initialization code for yascm.
;; Copyright (C) 2015 Hmgle <dustgle@gmail.com>
;;
;; This program is free software: you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful, but
;; WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;; General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program.  If not, see <http://www.gnu.org/licenses/>.

(define (caar x) (car (car x)))
(define (cadr x) (car (cdr x)))
(define (cdar x) (cdr (car x)))
(define (cddr x) (cdr (cdr x)))
(define (caaar x) (car (car (car x))))
(define (caadr x) (car (car (cdr x))))
(define (cadar x) (car (cdr (car x))))
(define (caddr x) (car (cdr (cdr x))))
(define (cdaar x) (cdr (car (car x))))
(define (cdadr x) (cdr (car (cdr x))))
(define (cddar x) (cdr (cdr (car x))))
(define (cdddr x) (cdr (cdr (cdr x))))
(define (caaaar x) (car (car (car (car x)))))
(define (caaadr x) (car (car (car (cdr x)))))
(define (caadar x) (car (car (cdr (car x)))))
(define (caaddr x) (car (car (cdr (cdr x)))))
(define (cadaar x) (car (cdr (car (car x)))))
(define (cadadr x) (car (cdr (car (cdr x)))))
(define (caddar x) (car (cdr (cdr (car x)))))
(define (cadddr x) (car (cdr (cdr (cdr x)))))
(define (cdaaar x) (cdr (car (car (car x)))))
(define (cdaadr x) (cdr (car (car (cdr x)))))
(define (cdadar x) (cdr (car (cdr (car x)))))
(define (cdaddr x) (cdr (car (cdr (cdr x)))))
(define (cddaar x) (cdr (cdr (car (car x)))))
(define (cddadr x) (cdr (cdr (car (cdr x)))))
(define (cdddar x) (cdr (cdr (cdr (car x)))))
(define (cddddr x) (cdr (cdr (cdr (cdr x)))))

(define (map proc items)
  (if (null? items)
    '()
    (cons (proc (car items))
          (map proc (cdr items)))))

(define (apply proc args)
  (eval
    (cons proc
          (map (lambda (x) (list 'quote x))
               args))))

(define (not x)
  (if x #f #t))

(define (less-list? elemlist)
  (if (or (null? elemlist) (null? (cdr elemlist)))
      #t
      (if (or (> (car elemlist) (cadr elemlist))
              (= (car elemlist) (cadr elemlist)))
          #f
          (less-list? (cdr elemlist)))))

(define (< . args) (less-list? args))

(define (less-or-eq-list? elemlist)
  (if (or (null? elemlist) (null? (cdr elemlist)))
      #t
      (if (> (car elemlist) (cadr elemlist))
          #f
          (less-or-eq-list? (cdr elemlist)))))

(define (<= . args) (less-or-eq-list? args))

(define (greater-or-eq-list? elemlist)
  (if (or (null? elemlist) (null? (cdr elemlist)))
      #t
      (if (> (cadr elemlist) (car elemlist))
          #f
          (greater-or-eq-list? (cdr elemlist)))))

(define (>= . args) (greater-or-eq-list? args))

(define (zero? x) (= 0 x))
(define (positive? x) (> x 0))
(define (negative? x) (> 0 x))

(define (remainder x y)
   (- x (* y (quotient x y))))

(define (modulo x y)
   (cond ((or (and (positive? x) (negative? y))
              (and (positive? y) (negative? x)))
                  (if (zero? (remainder x y)) 0 (+ y (remainder x y))))
         (else (remainder x y))))

(define (odd? x)
   (if (zero? (remainder x 2))
       #f #t))
(define (even? x) (not (odd? x)))

(define (abs x)
   (if (> 0 x)
       (- x)
       x))

(define (gcd a b)
   (if (= b 0)
       (abs a)
       (abs (gcd b (remainder a b)))))

(define (lcm a b) (quotient (abs (* a b)) (gcd a b)))

(define (extremum-list compar-proc maxelem elemlist)
  (if (null? elemlist)
      maxelem
      (if (compar-proc (car elemlist) maxelem)
          (extremum-list compar-proc (car elemlist) (cdr elemlist))
          (extremum-list compar-proc maxelem (cdr elemlist)))))

(define (max head . args) (extremum-list > head args))
(define (min head . args) (extremum-list < head args))

(define (length items)
  (define (length-iter a count)
    (if (null? a)
        count
        (length-iter (cdr a) (+ 1 count))))
  (length-iter items 0))

(define (for-each f l)
  (cond ((not (null? l))
         (let ((unused (f (car l))))
           (for-each f (cdr l))))))

(define (write x) (display x))

(define true #t)
(define false #f)

(define (equal? a b)
  (cond ((and (pair? a) (pair? b))
         (and (equal? (car a) (car b)) 
             (equal? (cdr a) (cdr b))))
        ((and (not (pair? a)) (not (pair? b)))
         (eq? a b))
        (else false)))

(define (memq item x)
  (cond ((null? x) false)
        ((eq? item (car x)) x)
        (else (memq item (cdr x)))))

;; The eqv? is exactly the same as the eq? predicate,
;; except that it will always return #t for same primitive values.
(define eqv? eq?)
(define memv memq)

(define (member item x)
  (cond ((null? x) false)
        ((equal? item (car x)) x)
        (else (member item (cdr x)))))

(define (append list1 list2)
   (if (null? list1)
       list2
       (cons (car list1) (append (cdr list1) list2))))

(define (reverse items) 
   (if (null? (cdr items)) 
       items 
       (append (reverse (cdr items)) 
               (cons (car items) '()))))

(define (list-ref items n)
  (if (= n 0)
      (car items)
      (list-ref (cdr items) (- n 1))))

;;;; generic-assoc
(define (generic-assoc cmp obj alst)
     (cond
          ((null? alst) #f)
          ((cmp obj (caar alst)) (car alst))
          (else (generic-assoc cmp obj (cdr alst)))))

(define (assq obj alst)
     (generic-assoc eq? obj alst))
(define (assv obj alst)
     (generic-assoc eqv? obj alst))
(define (assoc obj alst)
     (generic-assoc equal? obj alst))
