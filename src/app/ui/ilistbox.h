#pragma once
#include <utility>

namespace ui {

    template<typename HandleT>
    class IListBox
    {
    public:
        using Handle = HandleT;

        virtual ~IListBox() = default;

        /// Render the list box widget using ImGui.
        virtual void Show() = 0;

        /// Get the current selection handle (path, index, Verdict, etc.).
        virtual Handle GetSelectedHandle() const = 0;

        /// Resets the selection state and clears the "updated" flag.
        void ResetSelected()
        {
            m_isUpdated = false;
            ClearCurrentSelection();
        }

        /// Checks whether the selection has changed since the last call.
        bool IsSelectionUpdated()
        {
            return std::exchange(m_isUpdated, false);
        }

    protected:
        /// Mark selection as updated; call in derived when user clicks.
        void SetIsUpdated(bool isUpdated)
        {
            m_isUpdated = isUpdated;
        }

        /// Derived classes must clear their own selection state.
        virtual void ClearCurrentSelection() = 0;

    private:
        bool m_isUpdated = false;
    };

} // namespace ui
