;;;; modify from `r4rstest.scm' Test correctness of scheme implementations.
;; Copyright (C) 1991, 1992, 1993, 1994, 1995, 2000, 2003, 2004, 2006, 2007 Free Software Foundation, Inc.
;;
;; This program is free software: you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as
;; published by the Free Software Foundation, either version 3 of the
;; License, or (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful, but
;; WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;; General Public License for more details.
;;
;; You should have received a copy of the GNU General Public
;; License along with this program.  If not, see
;; <http://www.gnu.org/licenses/>.

;;;;"r4rstest.scm":  Test R4RS correctness of scheme implementations.
;;; Author:          Aubrey Jaffer
;;; Home-page:       http://swiss.csail.mit.edu/~jaffer/Scheme
;;; Current version: http://swiss.csail.mit.edu/ftpdir/scm/r4rstest.scm

;;; Modified by hmgle

(define cur-section '())
(define errs '())
(define SECTION (lambda args
		  (display "SECTION") (write args) (newline)
		  (set! cur-section args) #t))
(define record-error (lambda (e) (set! errs (cons (list cur-section e) errs))))

(define test
  (lambda (expect fun . args)
    (write (cons fun args))
    (display "  ==> ")
    ((lambda (res)
      (write res)
      (newline)
      (cond ((not (equal? expect res))
	     (record-error (list res expect (cons fun args)))
	     (display " BUT EXPECTED ")
	     (write expect)
	     (newline)
	     #f)
	    (else #t)))
     (if (procedure? fun) (apply fun args) (car args)))))

(define (report-errs)
  (newline)
  (if (null? errs) (display "Passed all tests")
      (begin
	(display "errors were:")
	(newline)
	(display "(SECTION (got expected (call)))")
	(newline)
	(for-each (lambda (l) (write l) (newline))
		  errs)))
  (newline))

(SECTION 2 1);; test that all symbol characters are supported.
'(+ - ... !.. $.+ %.- &.! *.: /:. :+. <-. =. >. ?. ~. _. ^.)

(SECTION 3 4)
(define disjoint-type-functions
  (list boolean? char? null? number? pair? procedure? string? symbol?))
(define type-examples
  (list
   #t #f #\a '() 9739 '(test) record-error "test" "" 'test '#() '#(a b c) ))
(define i 1)
(for-each (lambda (x) (display (make-string i #\ ))
		  (set! i (+ 3 i))
		  (write x)
		  (newline))
	  disjoint-type-functions)
(define type-matrix
  (map (lambda (x)
	 (let ((t (map (lambda (f) (f x)) disjoint-type-functions)))
	   (write t)
	   (write x)
	   (newline)
	   t))
       type-examples))
(SECTION 4 1 2)
(test '(quote a) 'quote (quote 'a))
(test '(quote a) 'quote ''a)
(SECTION 4 1 3)
(test 12 (if #f + *) 3 4)
(SECTION 4 1 4)
(test 8 (lambda (x) (+ x x)) 4)
(define reverse-subtract
  (lambda (x y) (- y x)))
(test 3 reverse-subtract 7 10)
(define add4
  (let ((x 4))
    (lambda (y) (+ x y))))
(test 10 add4 6)
(test '(3 4 5 6) (lambda x x) 3 4 5 6)
(test '(5 6) (lambda (x y . z) z) 3 4 5 6)
(SECTION 4 1 5)
(test 'yes 'if (if (> 3 2) 'yes 'no))
(test 'no 'if (if (> 2 3) 'yes 'no))
(test '1 'if (if (> 3 2) (- 3 2) (+ 3 2)))
(SECTION 4 1 6)
(define x 2)
(test 3 'define (+ x 1))
(set! x 4)
(test 5 'set! (+ x 1))
(SECTION 4 2 1)
(test 'greater 'cond (cond ((> 3 2) 'greater)
			   ((< 3 2) 'less)))
(test 'equal 'cond (cond ((> 3 3) 'greater)
			 ((< 3 3) 'less)
			 (else 'equal)))
(test #t 'and (and (= 2 2) (> 2 1)))
(test #f 'and (and (= 2 2) (< 2 1)))
(test '(f g) 'and (and 1 2 'c '(f g)))
(test #t 'and (and))
(test #t 'or (or (= 2 2) (> 2 1)))
(test #t 'or (or (= 2 2) (< 2 1)))
(test #f 'or (or #f #f #f))
(test #f 'or (or))
(test '(b c) 'or (or (memq 'b '(a b c)) (+ 3 0)))
(SECTION 4 2 2)
(test 6 'let (let ((x 2) (y 3)) (* x y)))
(test 35 'let (let ((x 2) (y 3)) (let ((x 7) (z (+ x y))) (* z x))))
(define x 34)
(test 5 'let (let ((x 3)) (define x 5) x))
(test 34 'let x)
(SECTION 4 2 3)
(define x 0)
(test 6 'begin (begin (set! x 5) (+ x 1)))
(SECTION 5 2 1)
(define add3 (lambda (x) (+ x 3)))
(test 6 'define (add3 3))
(define first car)
(test 1 'define (first '(1 2)))
(SECTION 5 2 2)
(test 45 'define
	(let ((x 5))
		(define foo (lambda (y) (bar x y)))
		(define bar (lambda (a b) (+ (* a b) a)))
		(foo (+ x 3))))
(define x 34)
(define (foo) (define x 5) x)
(test 5 foo)
(test 34 'define x)
(define foo (lambda () (define x 5) x))
(test 5 foo)
(test 34 'define x)
(define (foo x) ((lambda () (define x 5) x)) x)
(test 88 foo 88)
(test 4 foo 4)
(test 34 'define x)
(SECTION 6 1)
(test #f not #t)
(test #f not 3)
(test #f not (list 3))
(test #t not #f)
(test #f not '())
(test #f not (list))
(test #f not 'nil)

(test #t boolean? #f)
(test #f boolean? 0)
(test #f boolean? '())
(SECTION 6 2)
(test #t eq? 'a 'a)
(test #f eq? (list 'a) (list 'a))
(test #t eq? '() '())
(test #t eq? car car)
(let ((x '(a))) (test #t eq? x x))
(let ((x '#())) (test #t eq? x x))
(let ((x (lambda (x) x))) (test #t eq? x x))

(test #t equal? 'a 'a)
(test #t equal? '(a) '(a))
(test #t equal? '(a (b) c) '(a (b) c))
(test #t equal? "abc" "abc")
(test #t equal? 2 2)
(SECTION 6 3)
(test '(a b c d e) 'dot '(a . (b . (c . (d . (e . ()))))))

(test #t pair? '(a . b))
(test #t pair? '(a . 1))
(test #t pair? '(a b c))
(test #f pair? '())
; (test #f pair? '#(a b)) ; no plan to support vectors

(test '(a) cons 'a '())
(test '((a) b c d) cons '(a) '(b c d))
(test '("a" b c) cons "a" '(b c))
(test '(a . 3) cons 'a 3)
(test '((a b) . c) cons '(a b) 'c)

(test 'a car '(a b c))
(test '(a) car '((a) b c d))
(test 1 car '(1 . 2))

(test '(b c d) cdr '((a) b c d))
(test 2 cdr '(1 . 2))

(test '(a 7 c) list 'a (+ 3 4) 'c)
(test '() list)

(test 3 length '(a b c))
(test 3 length '(a (b) (c d e)))
(test 0 length '())

(test '(x y) append '(x) '(y))
(test '(a b c d) append '(a) '(b c d))
(test '(a (b) (c)) append '(a (b)) '((c)))
(test '() append)
(test '(a b c . d) append '(a b) '(c . d))
(test 'a append '() 'a)

(test '(c b a) reverse '(a b c))
(test '((e (f)) d (b c) a) reverse '(a (b c) d (e (f))))

(test 'c list-ref '(a b c d) 2)

(test '(a b c) memq 'a '(a b c))
(test '(b c) memq 'b '(a b c))
(test '#f memq 'a '(b c d))
(test '#f memq (list 'a) '(b (a) c))
(test '((a) c) member (list 'a) '(b (a) c))
(test '(101 102) memv 101 '(100 101 102))

(define e '((a 1) (b 2) (c 3)))
(test '(a 1) assq 'a e)
(test '(b 2) assq 'b e)
(test #f assq 'd e)
(test #f assq (list 'a) '(((a)) ((b)) ((c))))
(test '((a)) assoc (list 'a) '(((a)) ((b)) ((c))))
(test '(5 7) assv 5 '((2 3) (5 7) (11 13)))
(SECTION 6 4)
(test #t symbol? 'foo)
(test #t symbol? (car '(a b)))
(test #f symbol? "bar")
(test #t symbol? 'nil)
(test #f symbol? '())
(test #f symbol? #f)

(SECTION 6 5 5)
(test #t number? 3)

(test #t = 22 22 22)
(test #t = 22 22)
(test #f = 34 34 35)
(test #f = 34 35)
(test #t > 3 -6246)
(test #f > 9 9 -2424)
(test #t >= 3 -4 -6246)
(test #t >= 9 9)
(test #f >= 8 9)
(test #t < -1 2 3 4 5 6 7 8)
(test #f < -1 2 3 4 4 5 6 7)
(test #t <= -1 2 3 4 5 6 7 8)
(test #t <= -1 2 3 4 4 5 6 7)
(test #f < 1 3 2)
(test #f >= 1 3 2)

(test #t zero? 0)
(test #f zero? 1)
(test #f zero? -1)
(test #f zero? -100)
(test #t positive? 4)
(test #f positive? -4)
(test #f positive? 0)
(test #f negative? 4)
(test #t negative? -4)
(test #f negative? 0)
(test #t odd? 3)
(test #f odd? 2)
(test #f odd? -4)
(test #t odd? -1)
(test #f even? 3)
(test #t even? 2)
(test #t even? -4)
(test #f even? -1)

(test 38 max 34 5 7 38 6)
(test -24 min 3  5 5 330 4 -24)

(test 7 + 3 4)
(test '3 + 3)
(test 0 +)
(test 4 * 4)
(test 1 *)

(test -1 - 3 4)
(test -3 - 3)
(test 7 abs -7)
(test 7 abs 7)
(test 0 abs 0)

(test 5 quotient 35 7)
(test -5 quotient -35 7)
(test -5 quotient 35 -7)
(test 5 quotient -35 -7)
(test 1 modulo 13 4)
(test 1 remainder 13 4)
(test 3 modulo -13 4)
(test -1 remainder -13 4)
(test -3 modulo 13 -4)
(test 1 remainder 13 -4)
(test -1 modulo -13 -4)
(test -1 remainder -13 -4)
(define (divtest n1 n2)
	(= n1 (+ (* n2 (quotient n1 n2))
		 (remainder n1 n2))))
(test #t divtest 238 9)
(test #t divtest -238 9)
(test #t divtest 238 -9)
(test #t divtest -238 -9)

(test 4 gcd 0 4)
(test 4 gcd -4 0)
(test 4 gcd 32 -36)
; (test 0 gcd)
(test 288 lcm 32 -36)
; (test 1 lcm)

;;;;From: fred@sce.carleton.ca (Fred J Kaudel)
;;; Modified by jaffer.
(define (test-inexact)
  (SECTION 6 5 5)
  ;; test nothing
  (test #f not #t)
  (report-errs))

(define (test-bignum)
  (define tb
    (lambda (n1 n2)
      (= n1 (+ (* n2 (quotient n1 n2))
	       (remainder n1 n2)))))
  (newline)
  (display ";testing bignums; ")
  (newline)
  (SECTION 6 5 5)
  (test 0 modulo -2177452800 86400)
  (test 0 modulo 2177452800 -86400)
  (test 0 modulo 2177452800 86400)
  (test 0 modulo -2177452800 -86400)
  ;; no plan to support bignums
  ;; (test #t 'remainder (tb 281474976710655 65535))
  ;; (test #t 'remainder (tb 281474976710654 65535))
  ;; no plan to support "Numerical input and output" now
  ;; (SECTION 6 5 6)
  (report-errs))

;; no plan to support "Numerical input and output" now
;; (SECTION 6 5 6)

;; not support too much now
(SECTION 6 6)
; (test #t eqv? '#\  #\Space)
; (test #t eqv? #\space '#\Space)
(test #t char? #\a)
(test #t char? #\()
(test #t char? #\ )
(test #t char? '#\newline)

;; not support too much now
(SECTION 6 7)
(test #t string? "The word \"recursion\\\" has many meanings.")
(test #t string? "")

;; no plan to support vector
;; (SECTION 6 8)
;; (test #t vector? '#(0 (2 2 2 2) "Anna"))
;; (test #t vector? '#())
;; (test '#(a b c) vector 'a 'b 'c)
;; ; (test '#() vector)
;; (test 3 vector-length '#(0 (2 2 2 2) "Anna"))
;; (test 0 vector-length '#())
;; (test 8 vector-ref '#(1 1 2 3 5 8 13 21) 5)
;; (test '#(0 ("Sue" "Sue") "Anna") 'vector-set
;; 	(let ((vec (vector 0 '(2 2 2 2) "Anna")))
;; 	  (vector-set! vec 1 '("Sue" "Sue"))
;; 	  vec))
;; (test '#(hi hi) make-vector 2 'hi)
;; (test '#() make-vector 0)
;; (test '#() make-vector 0 'a)

(SECTION 6 9)
(test #t procedure? car)
(test #f procedure? 'car)
(test #t procedure? (lambda (x) (* x x)))
(test #f procedure? '(lambda (x) (* x x)))
;; no support call-with-current-continuation now
; (test #t call-with-current-continuation procedure?)
(test 7 apply + (list 3 4))
(test 7 apply (lambda (a b) (+ a b)) (list 3 4))
; (test 17 apply + 10 (list 3 4))
(test '() apply list '())

(test '(b e h) map cadr '((a b) (d e) (g h)))
;; not support multi-args map now
;; (test '(5 7 9) map + '(1 2 3) '(4 5 6))
;; (test '#(0 1 4 9 16) 'for-each
;; 	(let ((v (make-vector 5)))
;; 		(for-each (lambda (i) (vector-set! v i (* i i)))
;; 			'(0 1 2 3 4))
;; 		v))
; (test -3 call-with-current-continuation
; 		(lambda (exit)
; 		 (for-each (lambda (x) (if (negative? x) (exit x)))
; 		 	'(54 0 37 -3 245 19))
; 		#t))
; (define list-length
;  (lambda (obj)
;   (call-with-current-continuation
;    (lambda (return)
;     (letrec ((r (lambda (obj) (cond ((null? obj) 0)
; 				((pair? obj) (+ (r (cdr obj)) 1))
; 				(else (return #f))))))
; 	(r obj))))))
; (test 4 list-length '(1 2 3 4))
; (test #f list-length '(a b . c))
(test '() map cadr '())

;;; This tests full conformance of call-with-current-continuation.  It
;;; is a separate test because some schemes do not support call/cc
;;; other than escape procedures.  I am indebted to
;;; raja@copper.ucs.indiana.edu (Raja Sooriamurthi) for fixing this
;;; code.  The function leaf-eq? compares the leaves of 2 arbitrary
;;; trees constructed of conses.  
(define (next-leaf-generator obj eot)
  (letrec ((return #f)
	   (cont (lambda (x)
		   (recur obj)
		   (set! cont (lambda (x) (return eot)))
		   (cont #f)))
	   (recur (lambda (obj)
		      (if (pair? obj)
			  (for-each recur obj)
			  (call-with-current-continuation
			   (lambda (c)
			     (set! cont c)
			     (return obj)))))))
    (lambda () (call-with-current-continuation
		(lambda (ret) (set! return ret) (cont #f))))))
(define (leaf-eq? x y)
  (let* ((eot (list 'eot))
	 (xf (next-leaf-generator x eot))
	 (yf (next-leaf-generator y eot)))
    (letrec ((loop (lambda (x y)
		     (cond ((not (eq? x y)) #f)
			   ((eq? eot x) #t)
			   (else (loop (xf) (yf)))))))
      (loop (xf) (yf)))))
(define (test-cont)
  (newline)
  (display ";testing continuations; ")
  (newline)
  (SECTION 6 9)
  (test #t leaf-eq? '(a (b (c))) '((a) b c))
  (test #f leaf-eq? '(a (b (c))) '((a) b c d))
  (report-errs))

;;; Test Optional R4RS DELAY syntax and FORCE procedure
(define (test-delay)
  (newline)
  (display ";testing DELAY and FORCE; ")
  (newline)
  (SECTION 6 9)
  (test 3 'delay (force (delay (+ 1 2))))
  (test '(3 3) 'delay (let ((p (delay (+ 1 2))))
			(list (force p) (force p))))
  (test 2 'delay (letrec ((a-stream
			   (letrec ((next (lambda (n)
					    (cons n (delay (next (+ n 1)))))))
			     (next 0)))
			  (head car)
			  (tail (lambda (stream) (force (cdr stream)))))
		   (head (tail (tail a-stream)))))
  (letrec ((count 0)
	   (p (delay (begin (set! count (+ count 1))
			    (if (> count x)
				count
				(force p)))))
	   (x 5))
    (test 6 force p)
    (set! x 10)
    (test 6 force p))
  (test 3 'force
	(letrec ((p (delay (if c 3 (begin (set! c #t) (+ (force p) 1)))))
		 (c #f))
	  (force p)))
  (report-errs))

(define (test-sc4)
  (newline)
  (display ";testing scheme 4 functions; ")
  (newline)
  (SECTION 6 7)
  (test '(#\P #\space #\l) string->list "P l")
  ; (test '() string->list "")
  ; (test "1\\\"" list->string '(#\1 #\\ #\"))
  ; (test "" list->string '())
  (SECTION 6 8)
  (test '(dah dah didah) vector->list '#(dah dah didah))
  (test '() vector->list '#())
  (test '#(dididit dah) list->vector '(dididit dah))
  (test '#() list->vector '())
  (report-errs))

(report-errs)
(if (and (string->number "0.0") (inexact? (string->number "0.0")))
    (test-inexact))

(let ((n (string->number "281474976710655")))
  (if (and n (exact? n))
      (test-bignum)))
(newline)
(display "To fully test continuations, Scheme 4, and DELAY/FORCE do:")
(newline)
(display "(test-cont) (test-sc4) (test-delay)")
(newline)
"last item in file"

