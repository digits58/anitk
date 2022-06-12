;; configures include directories and the c++ standard library for linters
((c++-mode . ((eval . (set (make-local-variable 'project-path)
                           (expand-file-name (locate-dominating-file default-directory ".dir-locals.el"))))
              (eval . (set (make-local-variable 'project-includes)
                           (mapcar (lambda (p) (concat project-path p))
                                   '("include"
                                     "extern/imgui"
                                     "extern/imgui/backends"
                                     "extern/Catch2/src"
                                     "extern/glfw/include"
                                     "extern/stb"))))
              (eval . (set (make-local-variable 'clang-args)
                           (cons "-std=c++1z"
                                 (mapcar (lambda (p) (concat "-I" p)) project-includes))))
              (eval . (setq-local company-clang-arguments clang-args))
              (flycheck-clang-tidy-extra-options . "-extra-arg=-std=c++1z")
              (flycheck-clang-language-standard . "c++1z")
              (eval . (setq-local flycheck-clang-include-path project-includes)))))
