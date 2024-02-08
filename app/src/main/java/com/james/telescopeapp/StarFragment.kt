package com.james.telescopeapp

import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.recyclerview.widget.LinearLayoutManager
import com.james.telescopeapp.databinding.ActivityMainBinding
import com.james.telescopeapp.databinding.ActivityObjectSelectBinding
import com.james.telescopeapp.databinding.FragmentStarBinding

class StarFragment : Fragment() {

    private lateinit var binding: FragmentStarBinding
    private lateinit var db: StarDBHelper
    private lateinit var starAdapter: StarAdapter

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?,savedInstanceState: Bundle?): View? {
        //Creating View
        binding = FragmentStarBinding.inflate(layoutInflater)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        //After view is created
        super.onViewCreated(view, savedInstanceState)

        db = StarDBHelper(requireContext())
        starAdapter = StarAdapter(db.getAllStars(), requireContext())

        binding.starRecyclerView.layoutManager = LinearLayoutManager(requireContext())
        binding.starRecyclerView.adapter = starAdapter
    }

    override fun onResume() {
        super.onResume()
        starAdapter.refreshData(db.getAllStars())
    }
}