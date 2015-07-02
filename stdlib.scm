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
